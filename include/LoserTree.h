#pragma once
#include "defs.h"
#include "Record.h"

class LoserTree
{
public:
    LoserTree(Record runs[], int runCount);
    ~LoserTree();
    // Record* next();
private:
    void buildTree();
    int _runCount;
    int* _tree;
    Record* _runs;
}; // class LoserTree



class Run {
public:
    Run(int idx);
    ~Run();
    Record* peek();
    // Record* pop();
private:
    int _idx;
    Record *_r;
}; // class Run