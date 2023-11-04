#pragma once
#include "defs.h"
#include "Record.h"
#include "Run.h"

class LoserTree
{
	friend class ExternalMergeSortIterator;
public:
    LoserTree(Run **runs, int runCount);
    ~LoserTree();
    Record* next();
private:
    void buildTree();
    void replayGame(int idx, int prevWinner);
    void printTree();
    int _runCount;
    int* _tree;
    Run** _runs;
}; // class LoserTree
