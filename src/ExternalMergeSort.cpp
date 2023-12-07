#include "defs.h"
#include "LoserTree.h"
#include "ExternalMergeSort.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/task_info.h>
#include <mach/task.h>

// Function to get the current memory usage in bytes
long long getCurrentMemoryUsage() {
    task_vm_info_data_t vmInfo;
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;

    if (task_info(mach_task_self(), TASK_VM_INFO, (task_info_t)&vmInfo, &count) == KERN_SUCCESS) {
        return static_cast<long long>(vmInfo.phys_footprint);
    }

    return -1;  // Error occurred
}

// Function to get the total system memory in bytes
long long getTotalSystemMemory() {
    host_basic_info_data_t hostInfo;
    mach_msg_type_number_t count = HOST_BASIC_INFO_COUNT;

    if (host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&hostInfo, &count) == KERN_SUCCESS) {
        return static_cast<long long>(hostInfo.max_mem);
    }

    return -1;  // Error occurred
}

ExternalMergeSortPlan::ExternalMergeSortPlan(Plan* input) : _input(input) {}

ExternalMergeSortPlan::~ExternalMergeSortPlan() {
}

Iterator* ExternalMergeSortPlan::init() const {
    return new ExternalMergeSortIterator(this);
}

ExternalMergeSortIterator::ExternalMergeSortIterator(const ExternalMergeSortPlan* plan) 
    : _plan(plan), 
      _input(_plan->_input->init()),  
      _currentIdx(0),
      _degradedMode(false),
      _maxRunsInMemory(10)    
{
    // Step 1: Create Runs
    std::vector<Run*> records;
    Record *r;
    r = _input->next();

    _state = new RunStorageState();
    _tree = new MultiStageLoserTree(_state, r->size());
    while (r != nullptr) {
        DynamicRun *run = new DynamicRun(_state, CPU_CACHE_SIZE, r->rowSize);
        // Fill in the run array
        for (uint64_t i = 0; i < run->_maxRecords; i++) {
            run->push(r);
            r = _input->next();
        }
        run->sort();
        run->_readRemaining = run->_maxRecords;
        _tree->append(run);
    } 
    _tree->reduce();
    
    checkResourceThreshold();
}

ExternalMergeSortIterator::~ExternalMergeSortIterator() {
    TRACE(false);
    delete _input;
    delete _state;
    delete _tree;
}

Record* ExternalMergeSortIterator::next() {
    if (_degradedMode) {
        // Handle degraded mode
        if (_runsInMemory.empty()) {
            // If no runs are in memory, return nullptr indicating the end of data
            return nullptr;
        }
        // Get the next record from the runs in memory
        Record* nextRecord = _runsInMemory[_currentIdx]->next();
        if (nextRecord == nullptr) {
            // If the current run is exhausted, remove it from memory
            delete _runsInMemory[_currentIdx];
            _runsInMemory.erase(_runsInMemory.begin() + _currentIdx);
        } else {
            // Move to the next run in memory for the next iteration
            _currentIdx = (_currentIdx + 1) % _runsInMemory.size();
        }
        return nextRecord;
        
    } else {
        // Normal merge process
        return _tree->next();
    }
}

void ExternalMergeSortIterator::checkResourceThreshold() {
    // Example implementation for checking resource thresholds
    // You may need to adapt this based on your specific requirements or resource metrics

    // Get current memory usage
    long long memoryUsage = getCurrentMemoryUsage();

    // Set the memory threshold (example: 80% of available memory)
    long long memoryThreshold = getTotalSystemMemory() * 0.8;

    // Check if memory usage is above the threshold
    if (memoryUsage > memoryThreshold) {
        _degradedMode = true;
        std::cout << "Switching to degraded mode due to low memory." << std::endl;

        // Discard excess runs from memory
        while (_tree->_runCount > _maxRunsInMemory) {
            Run* discardedRun = _tree->discardRun();
            delete discardedRun;
        }

        // Move the remaining runs from the tree to memory
        _runsInMemory = _tree->getRuns();

        // Reset the current index to the start of the runs in memory
        _currentIdx = 0;

        // Clear thetree since all runs are now in memory
        _tree->clear();
    }
}