#pragma once
#include "defs.h"
#include "Record.h"
#include "Run.h"
#include <vector>

class LoserTree
{
	friend class ExternalMergeSortIterator;
public:
    LoserTree(std::vector<DynamicRun*>cacheOfRuns, int runCount);
    ~LoserTree();
    Record* next();
    Record *_last = nullptr;
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
    void append(DynamicRun *run);
    void reduce();
    void flushCacheRuns();
    void flushSSDRuns();

   
   

private:
    float _fanOutSSD = 0.95 * CACHE_SIZE / (CPU_CACHE_SIZE);
    float _fanOutHDD = 100; //Each SSD Run of 95 MB . MERGING A 100 will gIve us 9.5 GB (slightly less than 10 GB)i.e the SSD Capacity
    std::vector <DynamicRun*> _cacheOfRuns;
    std::vector <DynamicRun *> _HDDRuns;
    std::vector <DynamicRun *> _SSDRuns;
    RunStorageState *_state;
    LoserTree *_tree;
    size_t _recordSize;
    Record *_last;
    Record *_lastSSD;
    bool _first = true;
    bool _firstSSD = true;
}; // class LoserTree
