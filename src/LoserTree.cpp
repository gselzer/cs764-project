#include "LoserTree.h"
#include "Record.h"
#include <stdlib.h>
#include <iostream>

LoserTree::LoserTree(Run runs[], int runCount) : _runs(runs), _runCount(runCount){
    _tree = (int*) malloc(runCount * sizeof(int));
    buildTree();
};

LoserTree::~LoserTree() {
    free(_tree);
}

Record *LoserTree::next() {
    // Grab the overall winner
    int winner = _tree[0];
    Record* r = _runs[winner].pop();
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
    Record *rl = _runs[prevLoser].peek();
    Record *rw = _runs[prevWinner].peek();
    if (rl == nullptr) {
        // std::cout << "Index " << prevLoser << " is out of records - " <<  prevWinner << " wins \n";
        winner = prevWinner;
        loser = prevLoser;
    } else if (rw == nullptr) {
        // std::cout << "Index " << prevWinner << " is out of records - " <<  prevLoser << " wins \n";
        winner = prevLoser;
        loser = prevWinner;
    } else if (*rl <= *rw) {
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
    std::cout << "Building Tree...\n";
    int* _tmp = (int*) malloc(_runCount * sizeof(int));
    for(int i = 0; i < _runCount; i++) {
        _tmp[i] = i;
    }
    for (int half = _runCount / 2; half >= 1; half /= 2) {
        for(int i = 0; i < half; i++) {
            int idx1 = _tmp[2 * i];
            int idx2 = _tmp[2 * i + 1];
            if (*(_runs[idx1].peek()) <= *(_runs[idx2].peek())) {
                std::cout << "Index " << idx1 << " beat out Index " << idx2 << "\n";
                _tree[half + i] = idx2;
                _tmp[i] = idx1;
            } else {
                std::cout << "Index " << idx2 << " beat out Index " << idx1 << "\n";
                _tree[half + i] = idx1;
                _tmp[i] = idx2;
            }

        }
    }
    _tree[0] = _tmp[0];
    printTree();

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

Run::Run(int size) : _size(size), _idx(0) {
    // Allocate Record array
    _r = (Record *)malloc(_size * sizeof(Record));
    // Populate Record array
    int start = rand() % 5;
    for(int i = 0; i < _size; i++) {
        _r[i].row1 = start;
        _r[i].row2 = start;
        _r[i].row3 = start;
        start += rand() % 5;
    }

    // "Print" Record array
    std::cout << "Run data: ";
    for(int i = 0; i < _size; i++) {
        std::cout << _r[i].row1 << ", ";
    }
    std::cout << "\n";
}

Record *Run::peek() {
    return _r + _idx;
}

Record *Run::pop() {
    if (_idx < _size) {
        return _r + _idx++;
    }
    return nullptr;
}

Run::~Run() {
    free(_r);
}