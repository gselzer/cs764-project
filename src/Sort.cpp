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
    Record* r;
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
void sortHddAndStore(uint64_t totalDataSize, uint64_t hddOffset, const std::string& outputHdd) {
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

// Merge the sorted runs and store the merged data back onto the HDD
void mergeAndStoreRuns(const std::vector<Run*>& runs, const std::string& outputHdd) {
    std::vector<Record*> mergedRecords;
    for (Run* run : runs) {
        Record* record = run->next();
        while (record != nullptr) {
            mergedRecords.push_back(record);
            record = run->next();
        }
    }

    // Sort the merged records
    std::sort(mergedRecords.begin(), mergedRecords.end(), [](const Record* a, const Record* b) {
        // Implement your own comparison function here
        // return a->value < b->value;
    });

    // Store the sorted merged data back onto the HDDTo implement graceful degradation into the merge process, we can modify the `ExternalMergeSortIterator` class as follows:

```cpp
ExternalMergeSortIterator::ExternalMergeSortIterator(const ExternalMergeSortPlan* plan)
    : _plan(plan),
    _input(_plan->_input->init()),
    _currentIdx(0),
    _state(nullptr),
    _tree(nullptr)
{
    // Step 1: Create Runs
    std::vector<Run*> runs;
    Record* record = nullptr;
    uint64_t totalDataSize = 0;
    uint64_t dramOffset = 0;
    uint64_t ssdOffset = 0;
    uint64_t hddOffset = 0;
    bool isDramSorted = false;
    bool isSsdSorted = false;

    while ((record = _input->next()) != nullptr) {
        totalDataSize += record->size();

        // Check if the data can fit in DRAM
        if (totalDataSize <= dramLimit) {
            _dramRecords.push_back(record);
        }
        // Check if the data can fit in SSD
        else if (totalDataSize <= ssdLimit) {
            _ssdRecords.push_back(record);
        }
        else {
            // The data exceeds the available memory capacity
            // Sort and store the records in the appropriate storage medium
            if (!_dramRecords.empty()) {
                // Sort and store the records in DRAM
                sortDramAndStore(totalDataSize - record->size(), dramOffset, isSsdSorted, outputHdd);
                dramOffset += _dramRecords.size() * record->size();
                isDramSorted = true;
                runs.push_back(new InMemoryRun(_dramRecords));
                _dramRecords.clear();
            }
            else if (!_ssdRecords.empty()) {
                // Sort and store the records in SSD
                sortSsdAndStore(totalDataSize - record->size(), ssdOffset, isDramSorted, outputHdd);
                ssdOffset += _ssdRecords.size() * record->size();
                isSsdSorted = true;
                runs.push_back(new InMemoryRun(_ssdRecords));
                _ssdRecords.clear();
            }

            // Create a new HDD run for the current record
            runs.push_back(new HDDRun(record, hddOffset));
            hddOffset += record->size();
        }
    }

    // Sort and store any remaining records in the appropriate storage medium
    if (!_dramRecords.empty()) {
        sortDramAndStore(totalDataSize, dramOffset, isSsdSorted, outputHdd);
        runs.push_back(new InMemoryRun(_dramRecords));
        _dramRecords.clear();
    }
    else if (!_ssdRecords.empty()) {
        sortSsdAndStore(totalDataSize, ssdOffset, isDramSorted, outputHdd);
        runs.push_back(new InMemoryRun(_ssdRecords));
        _ssdRecords.clear();
    }

    // Merge the runs
    mergeAndStoreRuns(runs, outputHdd);

    // Initialize the state and tree
    _state = new RunStorageState();
    _tree = new MultiStageLoserTree(_state, runs.size());
}