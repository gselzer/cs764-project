#include "Run.h"
#include <string>
#include <iostream>
#include <stdexcept>
#include <exception>
#include <cstring>

// CacheSizedRun::CacheSizedRun(size_t recordSize):
//     bufSize((CPU_CACHE_SIZE) / recordSize),
//     _produce_idx(0),
//     _consume_idx(0),
//     _recordSize(recordSize)
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

void EmptyRun::push(Record *) {
    //nop
}

Record* EmptyRun::peek() {
    return nullptr;
}

Record* EmptyRun::pop() {
    return nullptr;
}

// FileBackedRun::FileBackedRun(RunStorageState *state, size_t recordSize): 
//     _produce_idx(0),
//     _consume_idx(0),
//     _readRemaining(0),
//     _recordSize(recordSize),
//     _bufSize((PAGE_SIZE) / recordSize)
// {
//     _last = new Record(_recordSize);
//     _state = state;
//     file = std::tmpfile();
//     buffer = (Record *) malloc(PAGE_SIZE);
// }

// FileBackedRun::~FileBackedRun() {
//     if (file!=nullptr){
//         std::fclose(file);
//     }
//     _state->read(_produce_idx * _recordSize, _onSSD);
//     free(buffer);
//     free(_last);
// }

// void FileBackedRun::push(Record * other) {
//     buffer[_produce_idx % _bufSize] = other;
//     _produce_idx++;
//     if (_produce_idx % _bufSize == 0) {
//         std::fwrite(buffer, _recordSize, _bufSize, file);
//     }
// }

// // This function should be called once all Records that this Run should store
// // have been pushed, and we are ready to start popping records. Before this
// // function is called, Records should not be popped, and after this function
// // is called, Records should not be pushed.
// void FileBackedRun::harden() {
//     std::fwrite(buffer, _recordSize, _produce_idx % _bufSize, file);
//     _onSSD = _state->write(_produce_idx * _recordSize);

//     std::cout << "Wrote " << _produce_idx << " Records to the file\n";
//     rewind(file);
// }

// Record *FileBackedRun::peek() {
//     if (_consume_idx < _produce_idx) {
//         if (_readRemaining == 0) {
//             _readRemaining += std::fread(buffer, _recordSize, _bufSize, file);
//         }
//         return buffer + (_consume_idx % _bufSize);
//     }
//     return nullptr;
// }

// Record *FileBackedRun::pop() {
//     if (_consume_idx < _produce_idx) {
//         if (_readRemaining == 0) {
//             _readRemaining += std::fread(buffer, _recordSize, _bufSize, file);
//             // std::cout << "Consume Index =" << _consume_idx << " - reading in " << _readRemaining << " more rows...\n";
//         }
//         *_last = (buffer + (_consume_idx % _bufSize));
//         _consume_idx++;
//         _readRemaining--;
//         return _last;
//     } return nullptr;
// }

RunStorageState::RunStorageState() : _ssdAllocated(0), _hddAllocated(0), _ssdTime(0), _hddTime(0) {
}

RunStorageState::~RunStorageState() {
    std::cout << "Total SSD latency + transfer time: " << _ssdTime << "\n"; 
    std::cout << "Total HDD latency + transfer time: " << _hddTime << "\n"; 
}

// Returns true iff "written to SSD"
bool RunStorageState::write(const int noBytes) {
   
    if (_ssdAllocated + noBytes <= _ssd_size ) {
         std::cout<<_ssdAllocated<<" / "<<_ssd_size<<"  - SSD State\n";
        // Write out to SSD
        std::cout << "Writing " << noBytes << " bytes to SSD\n";
        std::cout << "SSD allocated: " << _ssdAllocated << "\n";
        float transferTime = static_cast<float>(noBytes) / _ssd_bandwidth;
        _ssdTime += _ssd_latency + transferTime;
        _ssdAllocated += noBytes;
        return true;
    }
    else {
        // Write out to HDD
        std::cout << "Writing " << noBytes << " bytes to HDD\n"; 
        std::cout<<_hddAllocated<<" / "<< "Inf  - HDD State\n";   
        float transferTime = static_cast<float>(noBytes) / _hdd_bandwidth;
        _hddTime += _hdd_latency + transferTime;
        _hddAllocated += noBytes;
        return false;
    }
}

void RunStorageState::read(const int noBytes, const bool readFromSSD) {
    if (readFromSSD) {
        // Write out to SSD
        std::cout << "Reading " << noBytes << " bytes from SSD\n";
        float transferTime = static_cast<float>(noBytes) / _ssd_bandwidth;
        _ssdTime += _ssd_latency + transferTime;

        _ssdAllocated -= noBytes;
    }
    else {
        // Write out to HDD
        std::cout << "Reading " << noBytes << " bytes from HDD\n";
        float transferTime = static_cast<float>(noBytes) / _hdd_bandwidth;
        _hddTime += _hdd_latency + transferTime;

        _hddAllocated -= noBytes;
    }
}

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


DynamicRun::DynamicRun(RunStorageState *state, size_t pageSize, size_t rowSize):
    _pageSize(pageSize),
    _rowSize(rowSize),
    _recordSize(sizeof(Record) + 3 * rowSize),
    _produce_idx(0),
    _consume_idx(0),
    _state(state)
{
    _last = new Record(_recordSize);
    _maxRecords =  _pageSize / _recordSize;
    _records = (Record *) malloc(_maxRecords * sizeof(Record));
    _rows = new char[3 * _maxRecords * rowSize];
}

DynamicRun::~DynamicRun() {
    free(_records);
    delete _rows;
}

void DynamicRun::push(Record *r) {
     if (_produce_idx % _maxRecords == 0 && _produce_idx != 0) {
        if (file == nullptr) {
            file = std::tmpfile();
        }
        std::fwrite(_records, sizeof(Record), _maxRecords, file);
        std::fwrite(_rows, sizeof(char), 3 * _maxRecords * _rowSize, file);
        // std::cout << "Writing out " << _maxRecords << " Records to file\n";
    }
    // if (_produce_idx >= _maxRecords) {
    //     throw std::runtime_error("Cannot accept another record - this Run, which can store " + std::to_string(_maxRecords) + " is already full!\n");

    // }
    if(r == nullptr) {
        // std::cout << "Pushing null record\n";
        return;
    }
    int destIdx = _produce_idx % _maxRecords;
    char* rowIdx = _rows + (destIdx * 3 * _rowSize);
    memcpy(rowIdx, r->row1, _rowSize);
    _records[destIdx].row1 = rowIdx;

    rowIdx += _rowSize;
    memcpy(rowIdx, r->row2, _rowSize);
    _records[destIdx].row2 = rowIdx;

    rowIdx += _rowSize;
    memcpy(rowIdx, r->row3, _rowSize);
    _records[destIdx].row3 = rowIdx;
    _records[destIdx].rowSize = r->rowSize;

    _produce_idx++;
    delete r;

    
}

Record* DynamicRun::peek() {
    if (_consume_idx < _produce_idx) {
        if (_readRemaining == 0) {
            _readRemaining += std::fread(_records, sizeof(Record), _maxRecords, file);
            std::fread(_rows, sizeof(char), 3 * _maxRecords * _rowSize, file);
        }
        return _records + (_consume_idx % _maxRecords);
    }
    return nullptr;
}

Record *DynamicRun::pop() {
    if (_consume_idx < _produce_idx) {
        if (_readRemaining == 0) {
            _readRemaining += std::fread(_records, sizeof(Record), _maxRecords, file);
            std::fread(_rows, sizeof(char), 3 * _maxRecords * _rowSize, file);
            int totalBytes = (sizeof(Record) * _maxRecords) + (sizeof(char) * 3 * _maxRecords * _rowSize);
            std::cout << "Read in " << _maxRecords << " records from the file\n";

        }
        int srcIndex = _consume_idx % _maxRecords;
        char* rowIdx = _rows + (srcIndex * 3 * _rowSize);
        *_last = *(_records + srcIndex);
        // _last->row1 = rowIdx;
        // _last->row2 = rowIdx + _rowSize;
        // _last->row3 = rowIdx + _rowSize + _rowSize;
        _consume_idx++;
        _readRemaining--;
        // return _last;
        return new Record(*_last);

    }
    return nullptr;
}

// This function should be called once all Records that this Run should store
// have been pushed, and we are ready to start popping records. Before this
// function is called, Records should not be popped, and after this function
// is called, Records should not be pushed.
void DynamicRun::harden() {
    if (_produce_idx < _maxRecords) {
        _readRemaining = _produce_idx;
        return;
    }
    if (file == nullptr) {
        file = std::tmpfile();
    }
    std::cout << "Writing out " << _maxRecords << " Records to file\n";
    std::fwrite(_records, sizeof(Record), _maxRecords, file);
    std::fwrite(_rows, sizeof(char), 3 * _maxRecords * _rowSize, file);
    _onSSD = _state->write(_produce_idx * _recordSize);

    rewind(file);
}

void DynamicRun::sort() {
    // std::cout<<"Sorting "<<_produce_idx<<" records\n";
    if (!(_pageSize == CPU_CACHE_SIZE) && _produce_idx <= _maxRecords) {
        throw std::runtime_error("cannot sort a DynamicRun of size unless it is the CPU Cache size!\n");
    }
    int n = _produce_idx;
    // We're already sorted if we have 0 or 1 elements
    if (_produce_idx < 2) {
        return;
    }
    
    // Declare the quicksort function
    Record *tmp = new Record(3 * _rowSize + sizeof(Record));
    
    quicksort(0, n - 1, *tmp);

    // Step 2: Encode offset value
    _records[0].encodeOVC(nullptr);
    for (int i = 1; i < _produce_idx; i++) {
        _records[i].encodeOVC(_records + i - 1);
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
