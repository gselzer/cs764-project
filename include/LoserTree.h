#pragma once
#include "defs.h"
#include "Record.h"
#include "Run.h"
#include "Tree.h"

class LoserTree: public Tree
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

class MultiStageLoserTree: public Tree
{
	friend class ExternalMergeSortIterator;
public:
    MultiStageLoserTree(Run **runs, int runCount, RunStorageState *state);
    ~MultiStageLoserTree();
    Record* next();
private:
    Run **_runs;
    int _count;
}; // class LoserTree
