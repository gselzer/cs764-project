#pragma once
#include "defs.h"
#include "Record.h"
#include "Run.h"
#include <vector>

class LoserTree
{
	friend class ExternalMergeSortIterator;
public:
    LoserTree(std::vector<Run*>cacheOfRuns, int runCount);
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

class MultiStageLoserTree
{
	friend class ExternalMergeSortIterator;
public:
    MultiStageLoserTree(RunStorageState *state, size_t recordSize);
    ~MultiStageLoserTree();
    Record* next();
    void append(CacheSizedRun *run);
    void reduce();
    void flushCacheRuns();

private:
    float _fanOut = 0.9 * CACHE_SIZE / (CPU_CACHE_SIZE);
    std::vector <Run*> _cacheOfRuns;
    std::vector <Run *> _fileBackedRuns;
    RunStorageState *_state;
    LoserTree *_tree;
    size_t _recordSize;
}; // class LoserTree
