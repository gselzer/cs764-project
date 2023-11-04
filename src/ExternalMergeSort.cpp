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
    while (r != nullptr) {
        CacheSizedRun *run = new CacheSizedRun();
        // Fill in the run array
        for (int i = 0; i < RUN_RECORDS; i++) {
            // std::cout << "Adding run " << i << "\n";
            run->push(r);
            r = _input->next();
        }
        run->sort();
        records.push_back(run);
    } 
    
    // std::cout << "Done creating " << records.size() << " runs\n";

    // Step 2: Build the Loser Tree
    // Step 2.1: Obtain a run array that is a power of 2
    while (!IsPowerOf2(records.size())) {
        records.push_back(new EmptyRun());
    }
    Run ** runArray = (Run**) malloc(records.size() * sizeof(Run*));
    for(size_t i = 0; i < records.size(); i++) {
        runArray[i] = records[i];
    }
    
    // Step 2.2: Build a Run[]
    _tree = new LoserTree(runArray, records.size());


    // records.clear();
    // delete runArray;
}

ExternalMergeSortIterator::~ExternalMergeSortIterator() {
    TRACE(false);
    delete _input;
}

Record* ExternalMergeSortIterator::next() {
    return _tree->next();
}
