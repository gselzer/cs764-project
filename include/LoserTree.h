#pragma once
#include "defs.h"
#include "Record.h"

class Run {
public:
    Run(int size);
    ~Run();
    Record* peek();
    Record* pop();
private:
    int _size;
    int _idx;
    Record *_r;
}; // class Run

class LoserTree
{
public:
    LoserTree(Run runs[], int runCount);
    ~LoserTree();
    Record* next();
private:
    void buildTree();
    void replayGame(int idx, int prevWinner);
    void printTree();
    int _runCount;
    int* _tree;
    Run* _runs;
}; // class LoserTree
