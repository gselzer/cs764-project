
#include "Iterator.h"
# include "Record.h"
#include <vector>

class SortPlan : public Plan
{
	friend class SortIterator;
public:
	SortPlan (Plan * const input);
	~SortPlan ();
	Iterator * init () const;
private:
	Plan * _input;
}; // class SortPlan

class SortIterator : public Iterator
{
public:
	SortIterator (SortPlan const * const plan);
	~SortIterator ();
	Record* next();
	void sort(std::vector<Record*>& records);
private:
	SortPlan const * const _plan;
	Iterator * const _input;
	RowCount _consumed, _produced;
	std::vector<Record*> _sortedRecords;  // New Addition
    size_t _currentIdx;  // New Addition
}; // class SortIterator
