#include "../include/Iterator.h"
#include "../include/Scan.h"
#include "../include/Filter.h"
#include "../include/Sort.h"
#include "../include/VerifyOrder.h"
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
        if (record->row1 == -1) {  // Assuming -1 indicates the end
            break;
        }
        // Replace with your actual validation logic
        assert(record->row1 >= 0 && record->row2 >= 0);
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

    // Assuming SortPlan takes another Plan as input
    ScanPlan scanPlan(noRecords);  // Just a placeholder; replace with your actual input plan
    SortPlan sortPlan(&scanPlan);
    VerifyOrderPlan verifyPlan(&sortPlan);

    // Initialize SortIterator
    Iterator* sortIt = verifyPlan.init();
    
    while (true) {
        Record* record = sortIt->next();
        int val = record->row1;
        std::cout << val << "\n";
        delete record;
        if (val == -1) {
            break;
        }
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