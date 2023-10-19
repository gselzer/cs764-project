#pragma once
#include "defs.h"
#include "Record.h"

class Run {
public:
    Run(int size);
    ~Run();
    void push(Record *);
    void sort();
    Record* peek();
    Record* pop();
    int _produce_idx;
    int _consume_idx;
private:
    void merge(int l, int m, int r);
    int _size;
    Record **_r;
}; // class Run

class LoserTree
{
	friend class ExternalMergeSortIterator;
public:
    LoserTree(Run **runs, int runCount);
    ~LoserTree();
    Record* next();
private:
    void buildTree();
    void replayGame(int idx, int prevWinner);
    void printTree();
    int _runCount;
    int* _tree;
    Run** _runs;
}; // class LoserTree
