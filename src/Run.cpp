#include "Run.h"
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
        return new Record(_records + _consume_idx);
    }
    return nullptr;
}

Record *CacheSizedRun::pop() {
    if (_consume_idx < _produce_idx) {
        return new Record(_records + _consume_idx++);
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
    for (int curr_size = 1; curr_size <= n - 1; curr_size = 2 * curr_size) {
        for (int left_start = 0; left_start < n - 1; left_start += 2 * curr_size) {
            int mid = std::min(left_start + curr_size - 1, n - 1);
            int right_end = std::min(left_start + 2 * curr_size - 1, n - 1);
            merge(left_start, mid, right_end);
        }
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