#include "../include/Iterator.h"
#include "../include/Scan.h"
#include "../include/ExternalMergeSort.h"
#include "../include/Filter.h"
#include "../include/Sort.h"
#include "../include/VerifyOrder.h"
#include "../include/VerifyContent.h"
#include "../include/LoserTree.h"
#include "../include/Run.h"
#include <iostream>
#include <cassert>
#include <vector>

void testScanIterator(int numRecords) {
    std::cout << "Running ScanIterator tests...\n";
    
    ScanPlan scanPlan(numRecords);  // Assuming ScanPlan's constructor takes an integer argument for the number of records
    Iterator* scanIt = scanPlan.init();
    
    int count = 0;
    while (true) {
        Record* record = scanIt->next();
        if (!record) {  // Assuming -1 indicates the end
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

void testVerifyContentIterator(int numRecords) {
    std::cout << "Running SortIterator tests...\n";
    
    // Manually creating some records for testing


    // Assuming SortPlan takes another Plan as input
    ScanPlan scanPlan(numRecords);  // Just a placeholder; replace with your actual input plan
    VerifyContentState state;
    VerifyContentPlan verifyProducerPlan(&scanPlan, &state, true);
    ExternalMergeSortPlan sortPlan(&verifyProducerPlan);
    VerifyContentPlan verifyConsumerPlan(&sortPlan, &state, false);
    // VerifyOrderPlan verifyPlan(&sortPlan);

    // Initialize SortIterator
    Iterator* sortIt = verifyConsumerPlan.init();
    
    std::cout << "Getting next element\n";
    while (true) {
        Record* record = sortIt->next();
        if (record == nullptr) {
            break;
        }
        std::cout << *record << "\n";
        delete record;
    }
    
    // delete sortIt;
    delete sortIt;
    
    std::cout << "SortIterator tests passed.\n";
}

void testSortIterator(int numRecords) {
    std::cout << "Running SortIterator tests...\n";
    
    // Manually creating some records for testing
    std::cout << "Sorting " << numRecords << " records (" << numRecords * sizeof(Record) << " bytes)...\n";

    // Assuming SortPlan takes another Plan as input
    ScanPlan scanPlan(numRecords);  // Just a placeholder; replace with your actual input plan
    VerifyContentState state;
    VerifyContentPlan verifyProducerPlan(&scanPlan, &state, true);
    ExternalMergeSortPlan sortPlan(&verifyProducerPlan);
    VerifyOrderPlan validPlan(&sortPlan);
    VerifyContentPlan verifyConsumerPlan(&validPlan, &state, false);

    // Initialize SortIterator
    Iterator* sortIt = verifyConsumerPlan.init();
    
    std::cout << "Getting next element\n";
    int i = 0;
    while (true) {
        Record* record = sortIt->next();
        if (record == nullptr) {
            break;
        }
        // std::cout << i++ << ": " << *record << "\n";
    }
    
    delete sortIt;
    
    std::cout << "SortIterator tests passed.\n";
}

void testLoserTree() {
    // Record r[2] = {{1, 1, 1}, {2, 2, 2}};
    // Record r[4] = {{5, 1, 1}, {2, 2, 2}, {3, 3, 3}, {4, 4, 4}};
    // Record r[8] = {{1, 1, 1}, {2, 2, 2}, {3, 3, 3}, {4, 4, 4}, {5, 5, 5}, {6, 6, 6}, {7, 7, 7}, {8, 8, 8}};
    // Run r[4] = {10, 10, 10, 10};

    // LoserTree *l = new LoserTree(r, 4);
    // Record *r1 = l->next();
    // while (r1 != nullptr) {
    //     std::cout << "Pulled value : " << r1->row1 << "\n";
    //     r1 = l->next();
    // }

    // delete l;
}

void testRun(int numRecords) {
    
    CacheSizedRun * r = new CacheSizedRun();

    // Populate Record array
    Record * records = (Record *) malloc(numRecords * sizeof(Record));
    int start = rand() % 100;
    for(int i = 0; i < numRecords; i++) {
        records[i].row1 = start;
        records[i].row2 = start;
        records[i].row3 = start;
        r->push(records + i);
        std::cout << start << "\n";
        start = rand() % 100;
    }

    r->sort();

    // Assert multiple calls of peek return same type
    assert(r->peek() == r->peek());

    // Assert pop returns new values until end
    int lastValue = -1;
    for(int i = 0; i < numRecords; i++) {
        Record *rec = r->pop();
        // assert (rec->row1 >= lastValue);
        std::cout << rec->row1 << "\n";
        lastValue = rec->row1;
    }
    assert (r->pop() == nullptr);

    delete r;
    delete records;
}

void testFileBackedRun(int numRecords) {
    RunStorageState *state = new RunStorageState();
    FileBackedRun *r = new FileBackedRun(state);
    for(int i = 1; i <= numRecords; i++) {
        r->push(new Record(i, i, i));
    }

    r->harden();

    for(int i = 1; i <= numRecords; i++) {
        Record* rec = r->pop();
        assert(rec->row1 == i);
        free(rec);
    }
    assert(r->pop() == nullptr);


    free(r);
    free(state);
}

int main() {
    for (int i = 0; i < 24; i++) {
        int numRecords = 2 << i;
        // testScanIterator(numRecords);
        testSortIterator(numRecords);
        // testLoserTree(numRecords);
        // testRun(numRecords);
        // testFileBackedRun(numRecords);
    }
    
    std::cout << "All unit tests passed.\n";
    return 0;
}