#include "defs.h"
#include "ExternalMergeSort.h"
#include <stdlib.h>
#include <iostream>
#include <vector>


ExternalMergeSortPlan::ExternalMergeSortPlan(Plan* input) : _input(input) {}

ExternalMergeSortPlan::~ExternalMergeSortPlan() {
}

Iterator* ExternalMergeSortPlan::init() const {
    return new ExternalMergeSortIterator(this);
}

ExternalMergeSortIterator::ExternalMergeSortIterator(const ExternalMergeSortPlan* plan) 
    : _plan(plan), 
      _input(_plan->_input->init()),  
      _currentIdx(0)
{
    // Step 1: Create Runs
    std::vector<Run*> records;
    Record *r;
    r = _input->next();

    _state = new RunStorageState();
    _tree = new MultiStageLoserTree(_state, r->size());
    while (r != nullptr) {
        DynamicRun *run = new DynamicRun(_state,CPU_CACHE_SIZE,r->columnSize);
        // Fill in the run array
        for (uint64_t i = 0; i < run->_maxRecords; i++) {
            run->push(r);
            r = _input->next();
        }
        run->sort();
        run->_readRemaining = run->_maxRecords;
        // run->harden();
        // records.push_back(run);
        _tree->append(run);
    } 
    _tree->reduce();
    
}

ExternalMergeSortIterator::~ExternalMergeSortIterator() {
    TRACE(false);
    delete _input;
    delete _state;
    delete _tree;
}

Record* ExternalMergeSortIterator::next() {
    return _tree->next();
}


