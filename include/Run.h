#pragma once
#include "defs.h"
#include "Record.h"

#define RUN_BYTES 2 << 18
#define RUN_RECORDS (RUN_BYTES) / sizeof(Record)

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

// class FileBackedRun: public Run {
// public:
//     FileBackedRun(int size);
//     ~FileBackedRun();
//     void push(Record *);
//     void sort();
//     Record* peek();
//     Record* pop();
// private:
//     std::FILE *file;
// }; // class FileBackedRun