
#include "Iterator.h"
#include "LoserTree.h"
#include "Record.h"
#include "Run.h"
#include <vector>

class ExternalMergeSortPlan : public Plan
{
	friend class ExternalMergeSortIterator;
public:
	ExternalMergeSortPlan (Plan * const input);
	~ExternalMergeSortPlan ();
	void getCurrentMemoryUsage ();
	Iterator * init () const;
private:
	Plan * _input;
}; // class ExternalMergeSortPlan

class ExternalMergeSortIterator : public Iterator
{
public:
	ExternalMergeSortIterator (ExternalMergeSortPlan const * const plan);
	~ExternalMergeSortIterator ();
	Record* next();
	void sort(std::vector<Record*>& records);
	std::vector<Run*> _runsInMemory;
    void checkResourceThreshold();
private:

	ExternalMergeSortPlan const * const _plan;
	Iterator * const _input;
	RunStorageState *_state;
	RowCount _consumed, _produced;
    size_t _currentIdx;  // New Addition
	Run *_runs;
	MultiStageLoserTree *_tree;
	const int _maxRunsInMemory;
	bool _degradedMode;
}; // class ExternalMergeSortIterator
