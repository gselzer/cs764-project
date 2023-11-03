#include "LoserTree.h"
#include "Record.h"
#include <stdlib.h>
#include <iostream>

LoserTree::LoserTree(Run **runs, int runCount) : _runs(runs), _runCount(runCount){
    _tree = (int*) malloc(runCount * sizeof(int));
    buildTree();
    std::cout << "Loser Tree Constructed!";
};

LoserTree::~LoserTree() {
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
            if (r2 == nullptr || (r1 != nullptr && *r1 <= *r2)) {
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

Run::Run(int size) : _size(size), _produce_idx(0), _consume_idx(0) {
    // Keep track of Record array
    _r = (Record **) malloc(size * sizeof(Record *));
}

void Run::push(Record * record) {
    if (record != nullptr) {
        _r[_produce_idx++] = record;
    }
}

Record *Run::peek() {
    if (_consume_idx < _size) {
        return _r[_consume_idx];
    }
    return nullptr;
}

Record *Run::pop() {
    if (_consume_idx < _size) {
        return _r[_consume_idx++];
    }
    return nullptr;
}

Run::~Run() {
    free(_r);
}

void Run::sort() {
    int n = _produce_idx;
    if (_r[0] == nullptr) {
        return;
    }
    for (int curr_size = 1; curr_size <= n - 1; curr_size = 2 * curr_size) {
        for (int left_start = 0; left_start < n - 1; left_start += 2 * curr_size) {
            int mid = std::min(left_start + curr_size - 1, n - 1);
            int right_end = std::min(left_start + 2 * curr_size - 1, n - 1);
            merge(left_start, mid, right_end);
        }
    }

}

// Merge function for merge sort
void Run::merge(int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    // TODO: Don't malloc every time - malloc once at the beginning of the sort call
    Record ** L = (Record **)malloc(n1 * sizeof(Record *));
    Record ** R = (Record **)malloc(n2 * sizeof(Record *));
    
    for (int i = 0; i < n1; i++)
        L[i] = _r[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = _r[m + 1 + j];

    int i = 0;
    int j = 0;
    int k = l;

    while (i < n1 && j < n2) {
        if (*L[i] <= *R[j]) {
            _r[k] = L[i];
            i++;
        }
        else {
            _r[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        _r[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        _r[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}