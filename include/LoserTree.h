#pragma once
#include "defs.h"
#include "Record.h"

class LoserTree
{
public:
    LoserTree(Record runs[], int runCount);
    ~LoserTree();
    Record* next();
private:
    void buildTree();
    void replayGame(int idx);
    int _runCount;
    int* _tree;
    Record* _runs;
}; // class LoserTree



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