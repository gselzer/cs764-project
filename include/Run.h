#pragma once
#include "defs.h"
#include "Record.h"

// Keeps track of SSD and HDD occupancy.
// Also responsible for calculating total SSD/HDD transfer time
class RunStorageState
{
	public:
	RunStorageState();
	~RunStorageState();
	void write(const uint64_t noBytes, const int pageSize);
	void read(const uint64_t noBytes, const int pageSize);
	const float _ssd_latency = 0.0001; // 0.1 ms
	const float _hdd_latency = 0.01; // 10 ms
	const int _ssd_bandwidth = 100 * (2 << 19); // 100 MB/s
	const int _hdd_bandwidth = 100 * (2 << 19); // 100 MB/s
	const int _ssd_page_size = _ssd_latency * _ssd_bandwidth; // 
	const int _hdd_page_size = _hdd_latency * _hdd_bandwidth; // 
	private:
	const uint64_t _ssd_size = ((uint64_t) 10 )* (2 << 29); // 10 GB
	uint64_t _ssdAllocated;
    uint64_t _hddAllocated;
	float _ssdTime;
    float _hddTime;
}; // class RunStorageState

// Base Run class
class Run {
public:
    virtual ~Run() {};
    virtual void push(Record *) = 0;
    virtual Record* peek() = 0;
    virtual Record* pop() = 0;
}; // class Run

// Run implementation used to fill out LoserTree data structure to 2^n Runs
// Always returns nullptrs
class EmptyRun: public Run{ 
public:
    void push(Record *);
    Record* peek();
    Record* pop();
}; // class EmptyRun

// Run implementation actually containing runs
class DynamicRun: public Run {
public:
    DynamicRun(RunStorageState * state, size_t pageSize, size_t recordSize);
    ~DynamicRun();
    void push(Record *);
    Record* peek();
    Record* pop();
    void sort();
    void harden();
    size_t maxRecords;
    int readRemaining;
    size_t colSize;
    size_t recordSize;
private:
    void quicksort(int low, int high, Record &tmp);
    int partition(int low,int high, Record &tmp);
    std::FILE *file = nullptr;
    size_t _pageSize;
    Record* _records;
    char *_rows;
    int _produce_idx;
    int _consume_idx;
    RunStorageState *_state;
    Record *_last;
    Record *_lastSSD;
    int _no_failures=0;
}; // class DynamicRun

// Stale classes, kept for historical reasons.

// class CacheSizedRun: public Run {
// public:
//     CacheSizedRun(size_t recordSize);
//     ~CacheSizedRun();
//     void push(Record *);
//     void sort();
//     Record* peek();
//     Record* pop();
//     int bufSize;
// private:
//     void merge(int l, int m, int r);
//     void quicksort(int low, int high);
//     int partition(int low,int high);
//     Record *_records;
//     int _produce_idx;
//     int _consume_idx;
//     size_t recordSize;
// }; // class Run

// class FileBackedRun: public Run {
// public:
//     FileBackedRun(RunStorageState *state, size_t recordSize);
//     ~FileBackedRun();
//     void push(Record *);
//     void harden();
//     Record* peek();
//     Record* pop();
// private:
//     std::FILE *file;
//     Record *buffer;
//     int _produce_idx;
//     int _consume_idx;
//     int readRemaining;
//     Record *_last;
//     RunStorageState *_state;
//     bool _onSSD; 
//     int _bufSize;
//     size_t recordSize;
//     int _s;
// }; // class FileBackedRun
