#include "Run.h"
#include <iostream>
#include <exception>

CacheSizedRun::CacheSizedRun(): _produce_idx(0), _consume_idx(0) {
    _records = (Record *) malloc(RUN_BYTES);
}

void CacheSizedRun::push(Record * record) {
    if (record != nullptr) {
        _records[_produce_idx++] = *record;
        free(record);
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

void CacheSizedRun::sort() {
    int n = _produce_idx;
    // We're already sorted if we have 0 or 1 elements
    if (_produce_idx < 2) {
        return;
    }
    // Step 1: Internal Merge Sort
    for (int curr_size = 1; curr_size <= n - 1; curr_size = 2 * curr_size) {
        for (int left_start = 0; left_start < n - 1; left_start += 2 * curr_size) {
            int mid = std::min(left_start + curr_size - 1, n - 1);
            int right_end = std::min(left_start + 2 * curr_size - 1, n - 1);
            merge(left_start, mid, right_end);
        }
    }
    // Step 2: Encode offset value
    _records[0].encodeOVC(nullptr);
    for (int i = 1; i < _produce_idx; i++) {
        _records[i].encodeOVC(_records + i - 1);
    }
}

// Merge function for merge sort
void CacheSizedRun::merge(int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    // TODO: Don't malloc every time - malloc once at the beginning of the sort call
    Record * L = (Record *)malloc(n1 * sizeof(Record));
    Record * R = (Record *)malloc(n2 * sizeof(Record));
    
    for (int i = 0; i < n1; i++)
        L[i] = _records[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = _records[m + 1 + j];

    int i = 0;
    int j = 0;
    int k = l;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            _records[k] = L[i];
            i++;
        }
        else {
            _records[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        _records[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        _records[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
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
    _last = nullptr;
    _state = state;
    file = std::tmpfile();
    buffer = (Record *) malloc(PAGE_SIZE);
}

FileBackedRun::~FileBackedRun() {
    std::fclose(file);
    _state->read(_produce_idx * sizeof(Record), _onSSD);
    free(buffer);
    free(_last);
}

void FileBackedRun::push(Record * other) {
    buffer[_produce_idx % bufSize] = other;
    _last = other;
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
        // TODO: We may just want to return the actual pointer, not make a new record
        Record *r =new Record(buffer[_consume_idx % bufSize]);
        _consume_idx++;
        _readRemaining--;
        return r;
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
        float transferTime = ((double) noBytes) / _ssd_bandwidth;
        _ssdTime += _ssd_latency + transferTime;

        _ssdAllocated += noBytes;
        return true;
    }
    else {
        // Write out to HDD
        float transferTime = ((double) noBytes) / _hdd_bandwidth;
        _hddTime += _hdd_latency + transferTime;

        _hddAllocated += noBytes;
        return false;
    }
}

void RunStorageState::read(const int noBytes, const bool readFromSSD) {
    if (readFromSSD) {
        // Write out to SSD
        float transferTime = ((double) noBytes) / _ssd_bandwidth;
        _ssdTime += _ssd_latency + transferTime;

        _ssdAllocated -= noBytes;
    }
    else {
        // Write out to HDD
        float transferTime = ((double) noBytes) / _hdd_bandwidth;
        _hddTime += _hdd_latency + transferTime;

        _hddAllocated -= noBytes;
    }
}
