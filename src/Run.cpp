#include "Run.h"
#include <iostream>
#include <exception>

CacheSizedRun::CacheSizedRun(): _produce_idx(0), _consume_idx(0) {
  try {
    _records = (Record *) malloc(RUN_BYTES);
    } catch (const std::bad_alloc&) {
        // Handle memory allocation failure gracefully
        std::cerr << "Failed to allocate memory for _records.";
    }
}

void CacheSizedRun::push(Record * record) {
    if (record != nullptr) {
        if (_produce_idx < RUN_RECORDS){
            _records[_produce_idx++] = *record;
        delete record;
        } else {
            //Handle buffer overflow gracfeully
            std::cerr << "Buffer overflow";
        }
        
    }
}

Record *CacheSizedRun::peek() {
    if (_consume_idx < _produce_idx) {
        return _records + _consume_idx;
    }
    return nullptr;
}

Record *CacheSizedRun::pop() {
    if (_consume_idx < _produce_idx) {
        return _records + _consume_idx++;
    }
    return nullptr;
}

CacheSizedRun::~CacheSizedRun() {
    free(_records);   
} 

/**CacheSizedRun::~CacheSizedRun() {
    free(_records);   
}**/

void CacheSizedRun::sort() {
    std::cout<<"Sorting "<<_produce_idx<<" records\n";
    int n = _produce_idx;
    // We're already sorted if we have 0 or 1 elements
    if (_produce_idx < 2) {
        return;
    }
    
    // Declare the quicksort function

    quicksort(0, n - 1);

    // Step 2: Encode offset value
    _records[0].encodeOVC(nullptr);
    for (int i = 1; i < _produce_idx; i++) {
        _records[i].encodeOVC(_records + i - 1);
    }
}

void CacheSizedRun::quicksort(int low, int high) {
    if (low < high) {
        int pivot = partition(low, high);
        quicksort(low, pivot - 1);
        quicksort(pivot + 1, high);
    }
}

int CacheSizedRun::partition(int low, int high) {
    Record pivot = _records[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (_records[j] <= pivot) {
            i++;
            std::swap(_records[i], _records[j]);
        }
    }

    std::swap(_records[i + 1], _records[high]);
    return i + 1;
}

void EmptyRun::push(Record *) {
    //nop
}

Record* EmptyRun::peek() {
    return nullptr;
}

Record* EmptyRun::pop() {
    return nullptr;
}

FileBackedRun::FileBackedRun(RunStorageState *state): 
    _produce_idx(0),
    _consume_idx(0),
    _readRemaining(0)
{
    _last = new Record(0, 0, 0);
    _state = state;
    file = std::tmpfile();
    buffer = (Record *) malloc(PAGE_SIZE);
}

FileBackedRun::~FileBackedRun() {
    if (file!=nullptr){
        std::fclose(file);
    }
    _state->read(_produce_idx * sizeof(Record), _onSSD);
    free(buffer);
    free(_last);
}

void FileBackedRun::push(Record * other) {
    buffer[_produce_idx % bufSize] = other;
    _produce_idx++;
    if (_produce_idx % bufSize == 0) {
        std::fwrite(buffer, sizeof(Record), bufSize, file);
    }
}

// This function should be called once all Records that this Run should store
// have been pushed, and we are ready to start popping records. Before this
// function is called, Records should not be popped, and after this function
// is called, Records should not be pushed.
void FileBackedRun::harden() {
    std::fwrite(buffer, sizeof(Record), _produce_idx % bufSize, file);
    _onSSD = _state->write(_produce_idx * sizeof(Record));

    std::cout << "Wrote " << _produce_idx << " Records to the file\n";
    rewind(file);
}

Record *FileBackedRun::peek() {
    if (_consume_idx < _produce_idx) {
        if (_readRemaining == 0) {
            _readRemaining += std::fread(buffer, sizeof(Record), bufSize, file);
        }
        return buffer + (_consume_idx % bufSize);
    }
    return nullptr;
}

Record *FileBackedRun::pop() {
    if (_consume_idx < _produce_idx) {
        if (_readRemaining == 0) {
            _readRemaining += std::fread(buffer, sizeof(Record), bufSize, file);
            // std::cout << "Consume Index =" << _consume_idx << " - reading in " << _readRemaining << " more rows...\n";
        }
        *_last = (buffer + (_consume_idx % bufSize));
        _consume_idx++;
        _readRemaining--;
        return _last;
    } return nullptr;
}

RunStorageState::RunStorageState() : _ssdAllocated(0), _hddAllocated(0), _ssdTime(0), _hddTime(0) {
}

RunStorageState::~RunStorageState() {
    std::cout << "Total SSD latency + transfer time: " << _ssdTime << "\n"; 
    std::cout << "Total HDD latency + transfer time: " << _hddTime << "\n"; 
}

// Returns true iff "written to SSD"
bool RunStorageState::write(const int noBytes) {
    if (_ssd_size - _ssdAllocated > 0) {
        // Write out to SSD
        std::cout << "Writing " << noBytes << " bytes to SSD\n";
        float transferTime = static_cast<float>(noBytes) / _ssd_bandwidth;
        _ssdTime += _ssd_latency + transferTime;

        _ssdAllocated += noBytes;
        return true;
    }
    else {
        // Write out to HDD
        std::cout << "Writing " << noBytes << " bytes to HDD\n";    
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
