#pragma once
#include "defs.h"
#include "Record.h"

class Run {
public:
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
    CacheSizedRun();
    ~CacheSizedRun();
    void push(Record *);
    void sort();
    Record* peek();
    Record* pop();
private:
    void merge(int l, int m, int r);
    Record *_records;
    int _produce_idx;
    int _consume_idx;
}; // class Run

class RunStorageState
{
	public:
	RunStorageState();
	~RunStorageState();
	bool write(const int noBytes);
	void read(const int noBytes, const bool readFromSSD);
	private:
	const int _ssd_page_size = 4 * (2 << 9); // 4 KB
	const int _hdd_page_size = 8 * (2 << 9); // 8 KB
	const float _ssd_latency = 0.0001; // 0.1 ms
	const float _hdd_latency = 0.01; // 10 ms
	const int _ssd_bandwidth = 100 * (2 << 19); // 100 MB/s
	const int _hdd_bandwidth = 100 * (2 << 19); // 100 MB/s
	const long _ssd_size = 10 * (2 << 29); // 10 GB

	int _ssdAllocated, _hddAllocated;
	float _ssdTime, _hddTime;
};

class FileBackedRun: public Run {
public:
    FileBackedRun(RunStorageState *state);
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
    RunStorageState *_state;
    bool _onSSD; 
    const int bufSize = (PAGE_SIZE) / sizeof(Record);
}; // class FileBackedRun
