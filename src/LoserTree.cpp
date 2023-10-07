#include "LoserTree.h"
#include "Record.h"
#include <stdlib.h>
#include <iostream>

LoserTree::LoserTree(Record runs[], int runCount) : _runs(runs), _runCount(runCount){
    _tree = (int*) malloc(runCount * sizeof(int));
    buildTree();
};

LoserTree::~LoserTree() {
    free(_tree);
}

void LoserTree::buildTree() {
    int* _tmp = (int*) malloc(_runCount * sizeof(int));
    for(int i = 0; i < _runCount; i++) {
        _tmp[i] = i;
    }
    for (int half = _runCount / 2; half >= 1; half /= 2) {
        std::cout << half << "\n";
        for(int i = 0; i < half; i++) {
            int idx1 = _tmp[2 * i];
            int idx2 = _tmp[2 * i + 1];
            if (_runs[idx1] <= _runs[idx2]) {
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
    std::cout << "Initial Tree: [";
    for (int i = 0; i < _runCount; i++) {
        std::cout << _tree[i] << ",";
    }
    std::cout << "]";

    std::cout << "Index " << _tmp[0] << " won!\n";
    free(_tmp);
}