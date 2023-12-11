
#include "Iterator.h"
#include "LoserTree.h"
#include "Record.h"
#include "Run.h"
#include <vector>

// Plan responsible for computing an external merge sort
class ExternalMergeSortPlan : public Plan
{
	friend class ExternalMergeSortIterator;
public:
	ExternalMergeSortPlan (Plan * const input);
	~ExternalMergeSortPlan ();
	Iterator * init () const;
private:
	Plan * _input;
}; // class ExternalMergeSortPlan

// Iterator responsible for computing an external merge sort
class ExternalMergeSortIterator : public Iterator
{
public:
	ExternalMergeSortIterator (ExternalMergeSortPlan const * const plan);
	~ExternalMergeSortIterator ();
	Record* next();
	void sort(std::vector<Record*>& records);
private:
	ExternalMergeSortPlan const * const _plan;
	Iterator * const _input;
	RunStorageState *_state;
	RowCount _consumed, _produced;
    size_t _currentIdx;  // New Addition
	Run *_runs;
	MultiStageLoserTree *_tree;
}; // class ExternalMergeSortIterator
