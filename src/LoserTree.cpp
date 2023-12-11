#include "LoserTree.h"
#include "Run.h"
#include "Record.h"
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <vector>

LoserTree::LoserTree(std::vector<DynamicRun*>cacheOfRuns, int runCount){
    // Set up runs array (array of Run pointers)
    _runCount = runCount;
        
    // Increase run count to next highest power of 2
    while(!IsPowerOf2(_runCount)) {
        _runCount++;
    }
    // Allocate run array
    _runs = (Run **) malloc(_runCount * sizeof(Run *));
    // Fill in the beginning of the run array with the actual runs
    for(int i =0;i<runCount;i++){
        _runs[i] = cacheOfRuns[i];
    }
    // Pad the remainder with empty runs
    for(int i = runCount; i < _runCount; i++) {
        // std::cout << "Run Count: " << _runCount << "\n";
        _runs[i] = new EmptyRun();
    }
    // Set up tree data structure
    _tree = (int*) malloc(_runCount * sizeof(int));
    // Build the Tree
    buildTree();
}

LoserTree::~LoserTree() {
    for(int i = 0; i < _runCount; i++) {
        delete _runs[i];
    }
    free(_runs);
    free(_tree);
}

Record *LoserTree::next() {
    // Grab the overall winner
    int winner = _tree[0];
    Record* r = _runs[winner]->pop();
    // Determine the leaf node corresponding to winner's first comparison
    int startIdx = (_runCount + _tree[0]) / 2;
    // Replay the games from that tree leaf to root
    replayGame(startIdx, winner);
    // Return winner
    return r;
}

void LoserTree::replayGame(int idx, int prevWinner) {
    // Idx is 0 - all we have to do is assign the winner
    if (idx == 0) {
        _tree[idx] = prevWinner;
        // printTree();
        return;
    }
    // Otherwise, we need to do a comparison to determine
    // which run stays here (i.e. the lower) and which run
    // moves to the parent node (i.e. the winner)
    int prevLoser = _tree[idx];
    int winner = -1, loser = -1;
    Record *rl = _runs[prevLoser]->peek();
    Record *rw = _runs[prevWinner]->peek();
    if (rl == nullptr) {
        // std::cout << "Index " << prevLoser << " is out of records - " <<  prevWinner << " wins \n";
        winner = prevWinner;
        loser = prevLoser;
    } else if (rw == nullptr) {
        // std::cout << "Index " << prevWinner << " is out of records - " <<  prevLoser << " wins \n";
        winner = prevLoser;
        loser = prevWinner;
    } else if (rl->leOVC(rw)) {
        // std::cout << "Previous Loser " << *rl << " beat out Previous Winner " <<  *rw << "\n";
        rw->encodeOVC(rl);
        winner = prevLoser;
        loser = prevWinner;

    } else {
        // std::cout << "Previous Winner " << *rw << " beat out Previous Loser " <<  *rl << "\n";
        rl->encodeOVC(rw);
        winner = prevWinner;
        loser = prevLoser;
    }

    _tree[idx] = loser;
    replayGame(idx / 2, winner);
}

// Constructs the initial Tree of Losers structure
void LoserTree::buildTree() {
    int* _tmp = (int*) malloc(_runCount * sizeof(int));
    for(int i = 0; i < _runCount; i++) {
        _tmp[i] = i;
    }
    for (int half = _runCount / 2; half >= 1; half /= 2) {
        for(int i = 0; i < half; i++) {
            int idx1 = _tmp[2 * i];
            int idx2 = _tmp[2 * i + 1];
            Record *r1 = _runs[idx1]->peek();
            Record *r2 = _runs[idx2]->peek();
            if (r2 == nullptr || (r1 != nullptr && r1->leOVC(r2))) {
                if (r2 != nullptr) {
                    r2->encodeOVC(r1);
                }
                _tree[half + i] = idx2;
                _tmp[i] = idx1;
            } else {
                if (r1 != nullptr) {
                    r1->encodeOVC(r2);
                }
                _tree[half + i] = idx1;
                _tmp[i] = idx2;
            }

        }
    }
    _tree[0] = _tmp[0];
    // printTree();

    free(_tmp);
}

// Convenience function to print the tree
void LoserTree::printTree(){ 
    // Idx 0 is the "overall winner",
    // Idx 1 is the "root node" of the tree,
    // for Idx i>0, its children are located at (2i) and (2i+1)
    std::cout << "Tree: [";
    for (int i = 0; i < _runCount; i++) {
        std::cout << _tree[i] << ",";
    }
    std::cout << "]\n";
}

MultiStageLoserTree::MultiStageLoserTree(RunStorageState *state, size_t recordSize):
    _state(state),
    recordSize(recordSize)
{}

MultiStageLoserTree::~MultiStageLoserTree() {
    delete _tree;
    _HDDRuns.clear();
}

Record *MultiStageLoserTree::next() {
    return _tree->next();
}

// Adds run into the MultiStageLoserTree structure
void MultiStageLoserTree::append(DynamicRun *run ) {
    // Add the run into the structure
    _cacheOfRuns.push_back(run);
    // If DRAM is full, flush all runs in DRAM to ssd
    if(_cacheOfRuns.size()>_fanOutSSD){
        flushCacheRuns();
    }    
}

// Reduces down to a single LoserTree containing all data
void MultiStageLoserTree::reduce() {
    // Shortcut - if we only have data in DRAM, just build a LoserTree on that
    if(_cacheOfRuns.size()<_fanOutSSD && _SSDRuns.size()==0 && _HDDRuns.size()==0){
        std::cout<<"As Number of Records are low so no need to Spill to Disk\n";
        _tree = new LoserTree(_cacheOfRuns, _cacheOfRuns.size());
        
    }
    // Otherwise:
    else{
        // Flush Runs in DRAM to SSD
          if(_cacheOfRuns.size()>0){
            // std::cout<<"Flushing Cache Runs\n";
                flushCacheRuns();
          }
          // Flush Runs on SSD to HDD
            if(_SSDRuns.size()>0){
            // std::cout<<"Flushing SSD Runs\n";
                flushSSDRuns();
          }
    // Then - reduce down HDD runs to fit within a single Tree of Losers
    int _count = _HDDRuns.size();
    while(_count > _fanOutSSD) {
        int _storeIdx = 0;
        int _readIdx = 0;
        // Iterate across all runs
        while(_readIdx < _count) {
            // Construct a Loser Tree on _f consecutive runs
            int remainingRuns = _count - _readIdx;
            int numRuns = _fanOutSSD < remainingRuns ? _fanOutSSD : remainingRuns;

            std::vector<DynamicRun *> subVector (_HDDRuns.begin() + _readIdx, _HDDRuns.begin() + _readIdx + numRuns);
            LoserTree *tree = new LoserTree(subVector, numRuns);
            // Read out the sorted results to a file-backed run
            DynamicRun *run = new DynamicRun(_state,_state->_hdd_page_size, recordSize);
            Record *r = tree->next();
            // Temp pointer to enable OVC calculation
            while(r != nullptr) {
                // Put Record on Run
                run->push(r);
                r = tree->next();
            }
            // Store the new run
            run->harden();
            _HDDRuns[_storeIdx++] = run;
            // Increment readIdx
            _readIdx += numRuns;
            delete tree;
        }

        _count = _storeIdx;
        _HDDRuns.erase(_HDDRuns.begin() + _storeIdx, _HDDRuns.end());
    } 
    _tree = new LoserTree(_HDDRuns, _HDDRuns.size());
    }
}

// Flushes all runs in DRAM to SSD
void MultiStageLoserTree::flushCacheRuns(){
    LoserTree *tree = new LoserTree(_cacheOfRuns, _cacheOfRuns.size());
    // Read out the sorted results to a file-backed run
    DynamicRun *run = new DynamicRun(_state,_state->_ssd_page_size, _cacheOfRuns[0]->colSize);
    Record *r = tree->next();
    while(r != nullptr) {
        // Put Record on Run
        run->push(r);
        r = tree->next();
    }
    //store the FileBackedRun in the Vector
    run->harden();
    _SSDRuns.push_back(run);
    // If we now have a full SSD, flush to HDD
     if(_SSDRuns.size()>_fanOutHDD){
        flushSSDRuns();
    }    

    _cacheOfRuns.clear();

    delete tree;
}

// Flushes all runs on SSD to HDD
void MultiStageLoserTree::flushSSDRuns(){
    LoserTree *tree = new LoserTree(_SSDRuns, _SSDRuns.size());
    // Read out the sorted results to a file-backed run
    DynamicRun *run = new DynamicRun(_state,_state->_hdd_page_size, _SSDRuns[0]->colSize);
    Record *r = tree->next();
    while(r != nullptr) {
        // Put Record on Run
        run->push(r);
        r = tree->next();
    }
    //store the FileBackedRun in the Vector
    run->harden();
    _HDDRuns.push_back(run);
    
    _SSDRuns.clear();

    delete tree;
}