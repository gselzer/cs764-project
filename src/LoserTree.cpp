#include "LoserTree.h"
#include "Run.h"
#include "Record.h"
#include <stdlib.h>
#include <cstring>
#include <iostream>

LoserTree::LoserTree(Run **runs, int runCount){
    // Set up runs array (array of Run pointers)
    _runCount = runCount;
    while(!IsPowerOf2(_runCount)) {
        _runCount++;
    }
    _runs = (Run **) malloc(_runCount * sizeof(Run *));
    std::memcpy(_runs, runs, runCount * sizeof(Run *));
    for(int i = runCount; i < _runCount; i++) {
        _runs[i] = new EmptyRun();
    }
    // Set up tree data structure
    _tree = (int*) malloc(_runCount * sizeof(int));
    // Build the Tree
    buildTree();
    std::cout << "Loser Tree Constructed!";
};

LoserTree::~LoserTree() {
    for(int i = 0; i < _runCount; i++) {
        free(_runs[i]);
    }
    free(_runs);
    free(_tree);
}

Record *LoserTree::next() {
    // Grab the overall winner
    int winner = _tree[0];
    Record* r = _runs[winner]->pop();
    // std::cout << *r << " was the winner\n";
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
        // std::cout << "Index " << prevLoser << " beat out Index " <<  prevWinner << "\n";
        winner = prevLoser;
        loser = prevWinner;

    } else {
        // std::cout << "Index " << prevWinner << " beat out Index " <<  prevLoser << "\n";
        winner = prevWinner;
        loser = prevLoser;
    }

    _tree[idx] = loser;
    replayGame(idx / 2, winner);
}

void LoserTree::buildTree() {
    std::cout << "Building Tree with " << _runCount << " runs...\n";
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
                // std::cout << "Index " << idx1 << " beat out Index " << idx2 << "\n";
                _tree[half + i] = idx2;
                _tmp[i] = idx1;
            } else {
                // std::cout << "Index " << idx2 << " beat out Index " << idx1 << "\n";
                _tree[half + i] = idx1;
                _tmp[i] = idx2;
            }

        }
    }
    _tree[0] = _tmp[0];
    // printTree();

    free(_tmp);
}

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

MultiStageLoserTree::MultiStageLoserTree(Run **runs, int count, RunStorageState *state): _runs(runs), _count(count){

    int _f = 4; // 0.1 * CACHE_SIZE / (RUN_BYTES);
    // Keep going until we only have one run left
    while(_count > 1) {
        int _storeIdx = 0;
        int _readIdx = 0;
        // Iterate across all runs
        while(_readIdx < _count) {
            // Construct a Loser Tree on _f consecutive runs
            int remainingRuns = _count - _readIdx;
            int numRuns = _f < remainingRuns ? _f : remainingRuns;
            LoserTree *tree = new LoserTree(_runs + _readIdx, numRuns);
            // Read out the sorted results to a file-backed run
            FileBackedRun *run = new FileBackedRun(state);
            Record *r = tree->next();
            // Temp pointer to enable OVC calculation
            Record *tmp = nullptr;
            int i = 0;
            while(r != nullptr) {
                r->encodeOVC(tmp);
                std::cout << *r << "\n";
                // Put Record on Run
                run->push(r);

                // Get new Record
                tmp = r;
                r = tree->next();
                i++;
            }
            // Store the new run
            run->harden();
            _runs[_storeIdx++] = run;
            // Increment readIdx
            _readIdx += numRuns;
            // Update count - we removed numRuns runs, but added one more.
        }
        _count -= (_storeIdx * (_f - 1));
    }
}

MultiStageLoserTree::~MultiStageLoserTree() {

}

Record *MultiStageLoserTree::next() {
    return _runs[0]->pop();
}
