#include "Iterator.h"

// Plan creating a Filter iterator
class FilterPlan : public Plan
{
	friend class FilterIterator;
public:
	FilterPlan (Plan * const input);
	~FilterPlan ();
	Iterator * init () const;
private:
	Plan * const _input;
}; // class FilterPlan

// An iterator that filters the outputs of the previous iterator
class FilterIterator : public Iterator
{
public:
	FilterIterator (FilterPlan const * const plan);
	~FilterIterator ();
	Record* next ();
private:
	FilterPlan const * const _plan;
	Iterator * const _input;
	RowCount _consumed, _produced;
}; // class FilterIterator
