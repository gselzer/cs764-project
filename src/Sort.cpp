#include "defs.h"
#include "Sort.h"
#include "LoserTree.h"
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
}

    #include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

// Sort the data in DRAM and store the sorted data back onto the HDD
void sortDramAndStore(uint64_t totalDataSize, uint64_t dramOffset, bool isSsdSorted, const std::string& outputHdd) {
    // Read the data into memory
    std::vector<char> data(totalDataSize);
    readDataFromHDD(dramOffset, data);

    // Sort the data in DRAM
    std::sort(data.begin(), data.end());

    // Store the sorted data back onto the HDD
    writeDataToHDD(dramOffset, data, outputHdd);
}

// Sort the data in SSD and store the sorted data back onto the HDD
void sortSsdAndStore(uint64_t totalDataSize, uint64_t ssdOffset, bool isDramSorted, const std::string& outputHdd) {
    // Read the data into memory
    std::vector<char> data(totalDataSize);
    readDataFromSSD(ssdOffset, data);

    // Sort the data in SSD
    std::sort(data.begin(), data.end());

    // Store the sorted data back onto the HDD
    writeDataToHDD(ssdOffset, data, outputHdd);
}

// Sort the data in HDD and store the sorted data back onto the HDD
void sortHddAndStore(uint64_t totalDataSize, uint64_t hddOffset) {
    // Read the data into memory
    std::vector<char> data(totalDataSize);
    readDataFromHDD(hddOffset, data);

    // Sort the data in HDD
    std::sort(data.begin(), data.end());

    // Store the sorted data back onto the HDD
    writeDataToHDD(hddOffset, data, outputHdd);
}

// Helper functions to read and write data from/to HDD and SSD

void readDataFromHDD(uint64_t offset, std::vector<char>& data) {
    // Read data from HDD at the specified offset into the data vector
    // Example: using std::ifstream to read binary data from a file
    std::ifstream input("input.hdd", std::ios::binary);
    input.seekg(offset);
    input.read(data.data(), data.size());
    input.close();
}

void readDataFromSSD(uint64_t offset, std::vector<char>& data) {
    // Read data from SSD at the specified offset into the data vector

    // Example placeholder implementation:
    std::cout << "Reading data from SSD at offset " << offset << std::endl;
    // ...
}

void writeDataToHDD(uint64_t offset, const std::vector<char>& data, const std::string& outputHdd) {
    // Write the sorted data to HDD at the specified offset
    // Example: using std::ofstream to write binary data to a file
    std::ofstream output(outputHdd, std::ios::binary | std::ios::app);
    output.seekp(offset);
    output.write(data.data(), data.size());
    output.close();
}
    
    // Get the total data size
    uint64_t totalDataSize = 0; 
    while (r != nullptr) {
        totalDataSize += r->size();
        r = _input->next();
    }


    // Define the limits for DRAM and SSD
    const uint64_t dramLimit = CACHE_SIZE;
    const uint64_t ssdLimit = 0.95 * CACHE_SIZE / (CPU_CACHE_SIZE);

    // Perform sorting and storage based on the total data size
    if (totalDataSize < dramLimit) {
        // Case 1: Unsorted HDD -> DRAM -> HDD Sorted
        sortDramAndStore(totalDataSize, 0, false, "result/test_HDD.txt");
    } else if (totalDataSize < ssdLimit) {
        // Case 2: Unsorted HDD -> DRAM -> SSD Sorted, Sorted DRAM + Sorted SSD -> HDD Sorted
        sortSsdAndStore(totalDataSize, 0, false, "result/test_HDD.txt");
    } else {
        // Case 3: Unsorted HDD -> DRAM -> SSD Sorted, Sorted DRAM + Sorted SSD -> HDD Sorted, Sorted DRAM + Sorted SSD + Sorted HDD -> HDD Sorted
        sortHddAndStore(totalDataSize, 0);
    }
    
    
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
    _tree->reduce();
    


ExternalMergeSortIterator::~ExternalMergeSortIterator() {
    TRACE(false);
    delete _input;
    delete _state;
    delete _tree;
}

Record* ExternalMergeSortIterator::next() {
    return _tree->next();
}