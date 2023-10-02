#include "Sort.h"
#include <vector>

// Merge function for merge sort
void merge(std::vector<Record*>& records, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    std::vector<Record*> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = records[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = records[m + 1 + j];

    int i = 0;
    int j = 0;
    int k = l;

    while (i < n1 && j < n2) {
        if (L[i]->key <= R[j]->key) {
            records[k] = L[i];
            i++;
        }
        else {
            records[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        records[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        records[k] = R[j];
        j++;
        k++;
    }
}

// In-memory merge sort
void SortIterator::sort(std::vector<Record*>& records) {
    int n = records.size();
    for (int curr_size = 1; curr_size <= n - 1; curr_size = 2 * curr_size) {
        for (int left_start = 0; left_start < n - 1; left_start += 2 * curr_size) {
            int mid = std::min(left_start + curr_size - 1, n - 1);
            int right_end = std::min(left_start + 2 * curr_size - 1, n - 1);
            merge(records, left_start, mid, right_end);
        }
    }
}

SortPlan::SortPlan(Plan* input) : _input(input) {}

SortPlan::~SortPlan() {
    delete _input;
}

Iterator* SortPlan::init() const {
    return new SortIterator(this);
}

SortIterator::SortIterator(const SortPlan* plan) 
    : _plan(plan), 
      _input(_plan->_input->init()),  
      _currentIdx(0)
{
    std::vector<Record*> records;
    Record* record = _input->next();
    while (record->key != -1) {
        records.push_back(record);
        record = _input->next();
    }
    sort(records);
    _sortedRecords.swap(records);
}

SortIterator::~SortIterator() {
    delete _input;
    for (Record* record : _sortedRecords) {
        delete record;
    }
}

Record* SortIterator::next() {
    if (_currentIdx < _sortedRecords.size()) {
        return _sortedRecords[_currentIdx++];
    } else {
        return new Record(-1, -1); // Stopping condition
    }
}
