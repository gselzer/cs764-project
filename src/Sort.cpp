#include "defs.h"
#include "Sort.h"
#include <stdlib.h>
#include <iostream>
#include <vector>


ExternalMergeSortPlan::ExternalMergeSortPlan(Plan* input) : _input(input) {}

ExternalMergeSortPlan::~ExternalMergeSortPlan() {
}

Iterator* ExternalMergeSortPlan::init() const {
    return new ExternalMergeSortIterator(this);
}

// Creates an Iterator that will return all of the elements of the previous iterator,
// but sorted.
ExternalMergeSortIterator::ExternalMergeSortIterator(const ExternalMergeSortPlan* plan) 
    : _plan(plan), 
      _input(_plan->_input->init()),  
      _currentIdx(0)
{
    // Step 1: Create Runs, appending each to the Multi-Stage Loser Tree
    std::vector<Run*> records;
    Record *r;
    r = _input->next();

    _state = new RunStorageState();
    _tree = new MultiStageLoserTree(_state, r->size());
    while (r != nullptr) {
        DynamicRun *run = new DynamicRun(_state,CPU_CACHE_SIZE,r->columnSize);
        // Fill in the run array
        for (uint64_t i = 0; i < run->maxRecords; i++) {
            run->push(r);
            r = _input->next();
        }
        run->sort();
        run->readRemaining = run->maxRecords;
        _tree->append(run);
    } 
    // Step 2: Reduce the Multi-Stage Loser Tree down to a single tree
    _tree->reduce();
    
}

ExternalMergeSortIterator::~ExternalMergeSortIterator() {
    TRACE(false);
    delete _input;
    delete _state;
    delete _tree;
}

// Gets the next Record, in sorted order, from the internal LoserTree.
Record* ExternalMergeSortIterator::next() {
    return _tree->next();
}


