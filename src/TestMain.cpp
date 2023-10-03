#include "../include/Iterator.h"
#include "../include/Scan.h"
#include "../include/Filter.h"
#include "../include/Sort.h"
#include <iostream>
#include <cassert>
#include <vector>

void testScanIterator() {
    std::cout << "Running ScanIterator tests...\n";
    
    ScanPlan scanPlan(10);  // Assuming ScanPlan's constructor takes an integer argument for the number of records
    Iterator* scanIt = scanPlan.init();
    
    int count = 0;
    while (true) {
        Record* record = scanIt->next();
        if (record->key == -1) {  // Assuming -1 indicates the end
            break;
        }
        // Replace with your actual validation logic
        assert(record->key >= 0 && record->value >= 0);
        ++count;
    }
    assert(count == 10);  // Assuming we generated 10 records
    
    delete scanIt;
    
    std::cout << "ScanIterator tests passed.\n";
}

void testSortIterator() {
    std::cout << "Running SortIterator tests...\n";
    
    // Manually creating some records for testing
    int noRecords = 100;
    std::vector<Record*> records;
    // for (int i = 0; i < noRecords; i++) {
    //     records.push_back(new Record(rand(), i));
    // }

    // Assuming SortPlan takes another Plan as input
    ScanPlan scanPlan(noRecords);  // Just a placeholder; replace with your actual input plan
    SortPlan sortPlan(&scanPlan);

    // Initialize SortIterator
    Iterator* sortIt = sortPlan.init();
    // dynamic_cast<SortIterator*>(sortIt)->sort(records);
    
    int lastKey = -1;
    while (true) {
        Record* record = sortIt->next();
        if (record->key == -1) {
            break;
        }
        
        // Validate that records are sorted by key
        std::cout << record->key << "\n";
        assert(record->key >= lastKey);
        lastKey = record->key;
        records.push_back(record);
    }
    
    // Clean up
    for (Record* record : records) {
        delete record;
    }
    // delete sortIt;
    
    std::cout << "SortIterator tests passed.\n";
}

int main() {
    testScanIterator();
    testSortIterator();
    
    std::cout << "All unit tests passed.\n";
    return 0;
}