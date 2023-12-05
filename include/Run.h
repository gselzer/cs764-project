#pragma once
#include "defs.h"
#include "Record.h"

class Run {
public:
    virtual ~Run() {};
    virtual void push(Record *) = 0;
    virtual Record* peek() = 0;
    virtual Record* pop() = 0;
}; // class Run

class EmptyRun: public Run{ 
public:
    void push(Record *);
    Record* peek();
    Record* pop();
};

class CacheSizedRun: public Run {
public:
    CacheSizedRun(size_t recordSize);
    ~CacheSizedRun();
    void push(Record *);
    void sort();
    Record* peek();
    Record* pop();
    int bufSize;
private:
    void merge(int l, int m, int r);
    void quicksort(int low, int high);
    int partition(int low,int high);
    Record *_records;
    int _produce_idx;
    int _consume_idx;
    size_t _recordSize;
}; // class Run

class RunStorageState
{
	public:
	RunStorageState();
	~RunStorageState();
	bool write(const int noBytes);
	void read(const int noBytes, const bool readFromSSD);
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
    
};

class FileBackedRun: public Run {
public:
    FileBackedRun(RunStorageState *state, size_t recordSize);
    ~FileBackedRun();
    void push(Record *);
    void harden();
    Record* peek();
    Record* pop();
private:
    std::FILE *file;
    Record *buffer;
    int _produce_idx;
    int _consume_idx;
    int _readRemaining;
    Record *_last;
    RunStorageState *_state;
    bool _onSSD; 
    int _bufSize;
    size_t _recordSize;
    int _s;
}; // class FileBackedRun

class DynamicRun: public Run {
public:
    DynamicRun(RunStorageState * state, size_t pageSize, size_t recordSize);
    ~DynamicRun();
    void push(Record *);
    Record* peek();
    Record* pop();
    void sort();
    void harden();
    size_t _maxRecords;
private:
    void quicksort(int low, int high, Record &tmp);
    int partition(int low,int high, Record &tmp);
    std::FILE *file = nullptr;
    size_t _pageSize;
    size_t _recordSize;
    size_t _rowSize;
    Record* _records;
    char *_rows;
    int _produce_idx;
    int _consume_idx;
    int _readRemaining;
    RunStorageState *_state;
    bool _onSSD; 
    Record *_last;
}; // class DynamicRun
