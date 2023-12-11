#pragma once
#include "defs.h"
#include "Record.h"
#include "Run.h"
#include <vector>

// Implements the standard Tree of Losers Priority Queue
class LoserTree
{
	friend class ExternalMergeSortIterator;
public:
    LoserTree(std::vector<DynamicRun*>cacheOfRuns, int runCount);
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

// Larger data structure that uses multiple Loser Tree structures to reduce many CPU Cache-Sized
// Runs down to a single Tree of Losers, which can be pulled from using MultiStageLoserTree::next
class MultiStageLoserTree
{
	friend class ExternalMergeSortIterator;
public:
    MultiStageLoserTree(RunStorageState *state, size_t recordSize);
    ~MultiStageLoserTree();
    Record* next();
    // Adds a new DynamicRun to the larger structure
    void append(DynamicRun *run);
    // Reduces all contained runs down to a single Tree of Losers
    void reduce();
    // Combines all CPU Cache-sized runs to a single Run on SSD
    void flushCacheRuns();
    // Combines all SSD-sized runs to a single Run on HDD
    void flushSSDRuns();
private:
    // CACHE_SIZE / CPU_CACHE_SIZE is the number of CPU-cache-sized runs that could fit in DRAM.
    // We set the fan-out to slightly less than this number to allow room for the output buffer,
    // instructions, etc.
    float _fanOutSSD = 0.95 * CACHE_SIZE / (CPU_CACHE_SIZE);
    // Each SSD run will be approximately 95 MB. If we wait until we have 100 of them, we'd be at
    // 9.5GB on the SSD - slightly less than the total capacity
    float _fanOutHDD = 100;
    // Vector holding runs on DRAM
    std::vector <DynamicRun*> _cacheOfRuns;
    // Vector holding runs on SSD
    std::vector <DynamicRun *> _HDDRuns;
    // Vector holding runs on HDD
    std::vector <DynamicRun *> _SSDRuns;
    RunStorageState *_state;
    LoserTree *_tree;
    size_t recordSize;
}; // class LoserTree
