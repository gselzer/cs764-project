#include "Run.h"
#include <string>
#include <iostream>
#include <stdexcept>
#include <exception>
#include <cstring>

void EmptyRun::push(Record *) {
    //nop
}

Record* EmptyRun::peek() {
    return nullptr;
}

Record* EmptyRun::pop() {
    return nullptr;
}

RunStorageState::RunStorageState() : _ssdAllocated(0), _hddAllocated(0), _ssdTime(0), _hddTime(0) {
}

RunStorageState::~RunStorageState() {
    std::cout << "Total SSD latency + transfer time: " << _ssdTime << "\n"; 
    std::cout << "Total HDD latency + transfer time: " << _hddTime << "\n"; 
}

// Records the time taken for a write to a device
void RunStorageState::write(const uint64_t noBytes, const int _pageSize) {
   
    if (_pageSize == _ssd_page_size ) {
         std::cout<<_ssdAllocated<<" / "<<_ssd_size<<"  - SSD State\n";
        // Write out to SSD
        std::cout << "Writing " << noBytes << " bytes to SSD\n";
        std::cout << "SSD allocated: " << _ssdAllocated << "\n";
        float transferTime = static_cast<float>(noBytes) / _ssd_bandwidth;
        _ssdTime += _ssd_latency + transferTime;
        _ssdAllocated += noBytes;
       
    }
    else if(_pageSize == _hdd_page_size) {
        // Write out to HDD
        std::cout << "Writing " << noBytes << " bytes to HDD\n"; 
        std::cout<<_hddAllocated<<" / "<< "Inf  - HDD State\n";   
        float transferTime = static_cast<float>(noBytes) / _hdd_bandwidth;
        if(transferTime < 0){
            std::cout<<"Transfer time is negative\n";
        }
        _hddTime += _hdd_latency + transferTime;
        _hddAllocated += noBytes;
        
    }
}

// Records the time taken for a read to a device
void RunStorageState::read(const uint64_t noBytes, const int pageSize) {
    if (pageSize == _ssd_page_size) {
        // Write out to SSD
        std::cout << "Reading " << noBytes << " bytes from SSD\n";
        float transferTime = static_cast<float>(noBytes) / _ssd_bandwidth;
        _ssdTime += _ssd_latency + transferTime;
        _ssdAllocated -= noBytes;
    }
    else if(pageSize == _hdd_page_size) {
        
        // Write out to HDD
        std::cout << "Reading " << noBytes << " bytes from HDD\n";
        float transferTime = static_cast<float>(noBytes) / _hdd_bandwidth;
        _hddTime += _hdd_latency + transferTime;
        _hddAllocated -= noBytes;
    }
}

DynamicRun::DynamicRun(RunStorageState *state, size_t pageSize, size_t columnSize):
    _pageSize(pageSize),
    colSize(columnSize),
    recordSize(sizeof(Record) + 3 * columnSize),
    _produce_idx(0),
    _consume_idx(0),
    _state(state)
{
  try {
    _last = new Record(recordSize);
    _lastSSD = new Record(recordSize);
    maxRecords =  _pageSize / recordSize;
    _records = new Record[maxRecords];
    _rows = new char[3 * maxRecords * columnSize];
  } catch (const std::bad_alloc &) {
    // Handle memory allocation failure gracefully
    std::cerr << "Failed to allocate memory for _records.";
  }
}

DynamicRun::~DynamicRun() {
    try {
        _state->read(_produce_idx * recordSize, _pageSize );
        delete _last;
        delete _lastSSD;
        delete[] _records;
        delete[] _rows;
    } catch (const std::exception &e) {
        std::cerr << "Exception occurred during destruction: " << e.what() << std::endl;
    }
    
}

// Pushes a record onto the end of the run, writing the buffer out to a file if necessary
void DynamicRun::push(Record *r) {
    if (_produce_idx % maxRecords == 0 && _produce_idx != 0) {
        if (file == nullptr) {
            file = std::tmpfile();
        }
        size_t w1 = std::fwrite(_records, sizeof(Record), maxRecords, file);
        if (w1 == maxRecords) {
            //    std::cerr << "Write was fine!\n";
        } else {
            std::cerr << "Tried to write out " << 3 * maxRecords * colSize << " elements, but wrote out (w2) " << w1 << " instead (Failure " << ++_no_failures << ")\n";
        }
        size_t w2 = std::fwrite(_rows, sizeof(char), 3 * maxRecords * colSize, file);
        if (w2 == 3 * maxRecords * colSize) {
            //    std::cerr << "Write was fine!\n";
        } else {
            std::cerr << "Tried to write out " << 3 * maxRecords * colSize << " elements, but wrote out (w2) " << w2 << " instead (Failure " << ++_no_failures << ")\n";
        }
        // std::cout << "Writing out " << maxRecords << " Records to file\n";
    }
    if(r == nullptr) {
        // std::cout << "Pushing null record\n";
        return;
    }
    int destIdx = _produce_idx % maxRecords;
    char* rowIdx = _rows + (destIdx * 3 * colSize);
    memcpy(rowIdx, r->col1, colSize);
    _records[destIdx].col1 = rowIdx;

    rowIdx += colSize;
    memcpy(rowIdx, r->col2, colSize);
    _records[destIdx].col2 = rowIdx;

    rowIdx += colSize;
    memcpy(rowIdx, r->col3, colSize);
    _records[destIdx].col3 = rowIdx;

    _records[destIdx].columnSize = r->columnSize;
    _records[destIdx]._offset = r->_offset;
    _records[destIdx]._value = r->_value;
    _produce_idx++;
    delete r;

    
}

// Peeks at the next Record in the Run, but does not remove it
// Reads new Records in from file if necessary
Record* DynamicRun::peek() {
    if (_consume_idx < _produce_idx) {
        if (readRemaining == 0) {
            readRemaining += std::fread(_records, sizeof(Record), maxRecords, file);
            std::fread(_rows, sizeof(char), 3 * maxRecords * colSize, file);
            // std::cout << "Read in " << maxRecords << " records from the file\n";
        }
        return _records + (_consume_idx % maxRecords);
    }
    return nullptr;
}

// Removes and returns the next record in the run, reading new Records in
// from file if necessary
Record *DynamicRun::pop() {
    if (_consume_idx < _produce_idx) {
        if (readRemaining == 0) {
            readRemaining += std::fread(_records, sizeof(Record), maxRecords, file);
            std::fread(_rows, sizeof(char), 3 * maxRecords * colSize, file);
            int totalBytes = (sizeof(Record) * maxRecords) + (sizeof(char) * 3 * maxRecords * colSize);
            // std::cout << "Read in " << maxRecords << " records from the file\n";

        }
        int srcIndex = _consume_idx % maxRecords;
        char* rowIdx = _rows + (srcIndex * 3 * colSize);
        _consume_idx++;
        readRemaining--;
        return new Record(_records[srcIndex]);

    }
    return nullptr;
}

// Writes out any unwritten Records to File, and resets the file pointer.
// 
// This function should be called once all Records that this Run should store
// have been pushed, and we are ready to start popping records. Before this
// function is called, Records should not be popped, and after this function
// is called, Records should not be pushed.
void DynamicRun::harden() {
    try {
        // TODO: Can the if be removed and the else just always happen?
        if (_produce_idx < maxRecords) {
            readRemaining = _produce_idx;
            return;
        }
        else {
            readRemaining = 0;
        }
        if (file == nullptr) {
            file = std::tmpfile();
        }
        std::cout << "Writing out " << _produce_idx << " Records to file\n";
        std::fwrite(_records, sizeof(Record), maxRecords, file);
        std::fwrite(_rows, sizeof(char), 3 * maxRecords * colSize, file);
        _state->write(_produce_idx * recordSize, _pageSize);

        rewind(file);
    } catch (const std::exception &e) {
    std::cerr << "Exception occurred during hardening: " << e.what() << std::endl;
    }
}

// Sorts the Run
// Only callable if this run is CPU Cache sized
void DynamicRun::sort() {
    // std::cout<<"Sorting "<<_produce_idx<<" records\n";
    if (!(_pageSize == CPU_CACHE_SIZE) && _produce_idx <= maxRecords) {
        throw std::runtime_error("cannot sort a DynamicRun of size unless it is the CPU Cache size!\n");
    }
    int n = _produce_idx;
    // We're already sorted if we have 0 or 1 elements
    if (_produce_idx < 2) {
        return;
    }
    try {
        
        // Declare the quicksort function
        Record *tmp = new Record(3 * colSize + sizeof(Record));
        quicksort(0, n - 1, *tmp);
        delete tmp;


        // Step 2: Encode offset value
        _records[0].encodeOVC(nullptr);
        for (int i = 1; i < _produce_idx; i++) {
            _records[i].encodeOVC(_records + i - 1);
        }
    } catch (const std::exception &e) {
        // Handle exception gracefully
        std::cerr << "Exception occurred during sorting: " << e.what() << std::endl;
    }
}

void DynamicRun::quicksort(int low, int high, Record &tmp) {
    if (low <= high) {
        int pivot = partition(low, high, tmp);
        quicksort(low, pivot - 1, tmp);
        quicksort(pivot + 1, high, tmp);
    }
}

int DynamicRun::partition(int low, int high, Record &tmp) {
    Record pivot = _records[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (_records[j] <= pivot) {
            i++;
            tmp = _records[i];
            _records[i] = _records[j];
            _records[j] = tmp;
        }
    }

    tmp = _records[i+1];
    _records[i+1] = _records[high];
    _records[high] = tmp;
    return i + 1;
}

// -- Old Classes, kept as comments for hysterical raisins -- //

// CacheSizedRun::CacheSizedRun(size_t recordSize):
//     bufSize((CPU_CACHE_SIZE) / recordSize),
//     _produce_idx(0),
//     _consume_idx(0),
//     recordSize(recordSize)
// {
//   try {
//     // _records = (Record *) malloc(RUN_BYTES);
//     } catch (const std::bad_alloc&) {
//         // Handle memory allocation failure gracefully
//         std::cerr << "Failed to allocate memory for _records.";
//     }
// }

// void CacheSizedRun::push(Record * record) {
//     if (record != nullptr) {
//         if (_produce_idx < bufSize){
//             _records[_produce_idx++] = *record;
//         delete record;
//         } else {
//             //Handle buffer overflow gracfeully
//             std::cerr << "Buffer overflow";
//         }
        
//     }
// }

// Record *CacheSizedRun::peek() {
//     if (_consume_idx < _produce_idx) {
//         return _records + _consume_idx;
//     }
//     return nullptr;
// }

// Record *CacheSizedRun::pop() {
//     if (_consume_idx < _produce_idx) {
//         return _records + _consume_idx++;
//     }
//     return nullptr;
// }

// CacheSizedRun::~CacheSizedRun() {
//     free(_records);   
// } 

// void CacheSizedRun::sort() {
//     std::cout<<"Sorting "<<_produce_idx<<" records\n";
//     int n = _produce_idx;
//     // We're already sorted if we have 0 or 1 elements
//     if (_produce_idx < 2) {
//         return;
//     }
    
//     // Declare the quicksort function

//     quicksort(0, n - 1);

//     // Step 2: Encode offset value
//     _records[0].encodeOVC(nullptr);
//     for (int i = 1; i < _produce_idx; i++) {
//         _records[i].encodeOVC(_records + i - 1);
//     }
// }

// void CacheSizedRun::quicksort(int low, int high) {
//     if (low < high) {
//         int pivot = partition(low, high);
//         quicksort(low, pivot - 1);
//         quicksort(pivot + 1, high);
//     }
// }

// int CacheSizedRun::partition(int low, int high) {
//     Record pivot = _records[high];
//     int i = low - 1;

//     for (int j = low; j < high; j++) {
//         if (_records[j] <= pivot) {
//             i++;
//             std::swap(_records[i], _records[j]);
//         }
//     }

//     std::swap(_records[i + 1], _records[high]);
//     return i + 1;
// }

// FileBackedRun::FileBackedRun(RunStorageState *state, size_t recordSize): 
//     _produce_idx(0),
//     _consume_idx(0),
//     readRemaining(0),
//     recordSize(recordSize),
//     _bufSize((PAGE_SIZE) / recordSize)
// {
//     _last = new Record(recordSize);
//     _state = state;
//     file = std::tmpfile();
//     buffer = (Record *) malloc(PAGE_SIZE);
// }

// FileBackedRun::~FileBackedRun() {
//     if (file!=nullptr){
//         std::fclose(file);
//     }
//     _state->read(_produce_idx * recordSize, _onSSD);
//     free(buffer);
//     free(_last);
// }

// void FileBackedRun::push(Record * other) {
//     buffer[_produce_idx % _bufSize] = other;
//     _produce_idx++;
//     if (_produce_idx % _bufSize == 0) {
//         std::fwrite(buffer, recordSize, _bufSize, file);
//     }
// }

// // This function should be called once all Records that this Run should store
// // have been pushed, and we are ready to start popping records. Before this
// // function is called, Records should not be popped, and after this function
// // is called, Records should not be pushed.
// void FileBackedRun::harden() {
//     std::fwrite(buffer, recordSize, _produce_idx % _bufSize, file);
//     _onSSD = _state->write(_produce_idx * recordSize);

//     std::cout << "Wrote " << _produce_idx << " Records to the file\n";
//     rewind(file);
// }

// Record *FileBackedRun::peek() {
//     if (_consume_idx < _produce_idx) {
//         if (readRemaining == 0) {
//             readRemaining += std::fread(buffer, recordSize, _bufSize, file);
//         }
//         return buffer + (_consume_idx % _bufSize);
//     }
//     return nullptr;
// }

// Record *FileBackedRun::pop() {
//     if (_consume_idx < _produce_idx) {
//         if (readRemaining == 0) {
//             readRemaining += std::fread(buffer, recordSize, _bufSize, file);
//             // std::cout << "Consume Index =" << _consume_idx << " - reading in " << readRemaining << " more rows...\n";
//         }
//         *_last = (buffer + (_consume_idx % _bufSize));
//         _consume_idx++;
//         readRemaining--;
//         return _last;
//     } return nullptr;
// }

// NB To preserve history, we have maintained the mergesort implmentation
// added by Nitigya.
//
// void CacheSizedRun::sort() {
//     int n = _produce_idx;
//     // We're already sorted if we have 0 or 1 elements
//     if (_produce_idx < 2) {
//         return;
//     }
//     // Step 1: Internal Merge Sort
//     for (int curr_size = 1; curr_size <= n - 1; curr_size = 2 * curr_size) {
//         for (int left_start = 0; left_start < n - 1; left_start += 2 * curr_size) {
//             int mid = std::min(left_start + curr_size - 1, n - 1);
//             int right_end = std::min(left_start + 2 * curr_size - 1, n - 1);
//             merge(left_start, mid, right_end);
//         }
//     }
//     // Step 2: Encode offset value
//     _records[0].encodeOVC(nullptr);
//     for (int i = 1; i < _produce_idx; i++) {
//         _records[i].encodeOVC(_records + i - 1);
//     }
// }
//
// // Merge function for merge sort
// void CacheSizedRun::merge(int l, int m, int r) {
//     int n1 = m - l + 1;
//     int n2 = r - m;

//     // TODO: Don't malloc every time - malloc once at the beginning of the sort call
//     Record * L = (Record *)malloc(n1 * sizeof(Record));
//     Record * R = (Record *)malloc(n2 * sizeof(Record));

//     for (int i = 0; i < n1; i++)
//         L[i] = _records[l + i];
//     for (int j = 0; j < n2; j++)
//         R[j] = _records[m + 1 + j];

//     int i = 0;
//     int j = 0;
//     int k = l;

//     while (i < n1 && j < n2) {
//         if (L[i] <= R[j]) {
//             _records[k] = L[i];
//             i++;
//         }
//         else {
//             _records[k] = R[j];
//             j++;
//         }
//         k++;
//     }

//     while (i < n1) {
//         _records[k] = L[i];
//         i++;
//         k++;
//     }

//     while (j < n2) {
//         _records[k] = R[j];
//         j++;
//         k++;
//     }

//     free(L);
//     free(R);
// }
