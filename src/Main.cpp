#include "../include/Iterator.h"
#include "../include/Scan.h"
#include "../include/Sort.h"
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
        // std::cout << i++ << ": " << record->col1[0] << "\n";
        delete record;
    }
    
    delete sortIt;
    
    // std::cout << "SortIterator tests passed.\n";
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

    // Output to trace file
    std::ofstream traceOut(traceFile);
    // override the cout stream buffer with a file stream buffer
    std::streambuf *cout_buffer = std::cout.rdbuf(); 
    // save the current buffer 
    std::cout.rdbuf(traceOut.rdbuf()); 
    // redirect cout to the output file 

    if (traceOut.is_open()) {
        //Main Function Working
        sort(numRecords, recordSize);
        traceOut.close();
        std::cout.rdbuf(cout_buffer); 
        // restore the original cout buffer 
    } else {
        std::cerr << "Failed to open trace file: " << traceFile << "\n";
    }

    std::cout << numRecords << " Records sorted successfully.\n";
    return 0;
}


