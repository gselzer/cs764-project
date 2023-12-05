#include "../include/Iterator.h"
#include "../include/Scan.h"
#include "../include/ExternalMergeSort.h"
#include "../include/Filter.h"
#include "../include/VerifyOrder.h"
#include "../include/VerifyContent.h"
#include "../include/LoserTree.h"
#include "../include/Run.h"
#include "../include/Record.h"
#include <iostream>
#include <cassert>
#include<limits>
#include <vector>
#include <getopt.h>
#include <filesystem>
#include <fstream>

void sort(RowCount numRecords, size_t recordSize) {
    // std::cout << "Running SortIterator tests...\n";
    
    // Manually creating some records for testing
    // std::cout << "Sorting " << numRecords << " records (" << numRecords * recordSize << " bytes)...\n";

    // Assuming SortPlan takes another Plan as input
    ScanPlan scanPlan(numRecords, recordSize);  // Just a placeholder; replace with your actual input plan
    VerifyContentState *state = new VerifyContentState(recordSize);
    VerifyContentPlan verifyProducerPlan(&scanPlan, state, true);
    ExternalMergeSortPlan sortPlan(&verifyProducerPlan);
    VerifyOrderPlan validPlan(&sortPlan, recordSize);
    VerifyContentPlan verifyConsumerPlan(&validPlan, state, false);

    // Initialize SortIterator
    Iterator* sortIt = verifyConsumerPlan.init();
    
    // std::cout << "Getting next element\n";
    int i = 0;
    while (true) {
        Record* record = sortIt->next();
        if (record == nullptr) {
            break;
        }
        // std::cout << "Got a record!\n";
        std::cout << i++ << ": " << record->row1[0] << "\n";
        delete record;
    }
    
    delete sortIt;
    
    // std::cout << "SortIterator tests passed.\n";
}

void testDynamicCacheSizedRun(RowCount numRecords, size_t recordSize) {
	size_t rowSize = (recordSize - sizeof(Record)) / 3 ;

    // Try a CPU Cache-sized run
    RunStorageState *state = new RunStorageState();
    DynamicRun *run = new DynamicRun(state, CPU_CACHE_SIZE, rowSize);
    std::vector<Record*> recs;

    for(size_t i = 0; i < numRecords; i++) {
        Record *r = new Record(recordSize);
        run->push(r);
        recs.push_back(r);
    }
    run->sort();
    run->harden();
	// TODO: Needs to be minimum value
    std::vector<Record*> sorted;
	Record *_last = new Record(recordSize);
	for(size_t i = 0; i < rowSize; i++){
		_last->row1[i] = 0;
		_last->row2[i] = 0;
		_last->row3[i] = 0;
	}
    for(size_t i = 0; i < numRecords; i++) {
        Record *r = run->pop();
        assert(*_last <= *r);
        *_last = *r;
        sorted.push_back(r);
    }
    assert(nullptr == run->peek());
    for(size_t i = 0; i < sorted.size(); i++) {
        bool _found = false;
        for (size_t j = 0; j < recs.size(); j++){
            if(*sorted[i] == *recs[j]) {
                _found = true;
                break;
            }
        }
        assert(_found);
    }
    std::cout << "All tests passed!\n";


    delete _last;
    delete run;
}

void testDynamicFileSizedRun(RowCount numRecords, size_t recordSize) {
	size_t rowSize = (recordSize - sizeof(Record)) / 3 ;
    RunStorageState *state = new RunStorageState();
    DynamicRun *run = new DynamicRun(state, state->_ssd_page_size, rowSize);
    // std::vector<Record*> recs;
    for(size_t i = 0; i < numRecords; i++) {
        Record *r = new Record(recordSize);
        // recs.push_back(r);
        run->push(r);
    }
    run->harden();
    std::vector<Record*> sorted;
    for(size_t i = 0; i < numRecords; i++) {
        Record *r = run->pop();
        // assert(*r == *recs[i]);
    }
    assert(nullptr == run->peek());
    std::cout << "All tests passed!\n";
    delete run;
}

int main(int argc, char *argv[]) {
  
    RowCount numRecords = 0;
    size_t recordSize = 0;
    std::string traceFile;

    int option;
    while ((option = getopt(argc, argv, "c:s:o:")) != -1) {
        switch (option) {
            case 'c':
                numRecords = std::stoull(optarg);
                break;
            case 's':
                recordSize = static_cast<size_t>(std::stoul(optarg));
                // std::cout << "Record size: " << recordSize << "\n";
                break;
            case 'o':
                traceFile = optarg;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " -c numRecords -s recordSize -o traceFile\n";
                return 1;
        }
    }

    
    if (numRecords == 0 || recordSize == 0 || traceFile.empty()) {
        std::cerr << "Invalid arguments\n";
        return 1;
    }

        // testScanIterator(numRecords);
        // testLoserTree(numRecords);
        // testRun(numRecords);
        // testFileBackedRun(numRecords);
    
    
 // Output to trace file
    std::ofstream traceOut(traceFile);
//  override the cout stream buffer with a file stream buffer
    std::streambuf *cout_buffer = std::cout.rdbuf(); 
    // save the current buffer 
    std::cout.rdbuf(traceOut.rdbuf()); 
    // redirect cout to the output file 

    if (traceOut.is_open()) {
        //Main Function Working
        sort(numRecords, recordSize);
    // if (recordSize * numRecords < CPU_CACHE_SIZE) {
    //     testDynamicCacheSizedRun(numRecords, recordSize);
    // } else {
    //     testDynamicFileSizedRun(numRecords, recordSize);
    // }
        traceOut.close();
        std::cout.rdbuf(cout_buffer); 
        // restore the original cout buffer 
    } else {
        std::cerr << "Failed to open trace file: " << traceFile << "\n";
    }

    std::cout << numRecords << " Records sorted successfully.\n";
    return 0;
}


