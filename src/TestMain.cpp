#include "../include/Iterator.h"
#include "../include/Scan.h"
#include "../include/Filter.h"
#include "../include/Sort.h"
#include "../include/VerifyOrder.h"
#include "../include/LoserTree.h"
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
    delete sortIt;
    
    std::cout << "SortIterator tests passed.\n";
}

void testLoserTree() {
    // Record r[2] = {{1, 1, 1}, {2, 2, 2}};
    // Record r[4] = {{5, 1, 1}, {2, 2, 2}, {3, 3, 3}, {4, 4, 4}};
    Record r[8] = {{1, 1, 1}, {2, 2, 2}, {3, 3, 3}, {4, 4, 4}, {5, 5, 5}, {6, 6, 6}, {7, 7, 7}, {8, 8, 8}};

    LoserTree *l = new LoserTree(r, 8);

    delete l;
}

void testRun() {
    int numRecords = 10;
    Run * r = new Run(numRecords);

    // Assert multiple calls of peek return same type
    assert(r->peek() == r->peek());

    // Assert pop returns new values until end
    int lastValue = -1;
    for(int i = 0; i < numRecords; i++) {
        Record *rec = r->pop();
        assert (rec->row1 >= lastValue);
        lastValue = rec->row1;
    }
    assert (r->pop() == nullptr);

    delete r;
}

int main() {
    // testScanIterator();
    // testSortIterator();
    // testLoserTree();
    testRun();
    
    std::cout << "All unit tests passed.\n";
    return 0;
}