
#include "Iterator.h"
# include "Record.h"
#include <vector>

class VerifyOrderPlan : public Plan
{
	friend class VerifyOrderIterator;
public:
	VerifyOrderPlan (Plan * const input, size_t const recordSize);
	~VerifyOrderPlan ();
	Iterator * init () const;
private:
	Plan * _input;
	size_t _recordSize;
}; // class VerifyOrderPlan

class VerifyOrderIterator : public Iterator
{
public:
	VerifyOrderIterator (VerifyOrderPlan const * const plan);
	~VerifyOrderIterator ();
	Record* next();
private:
	VerifyOrderPlan const * const _plan;
	Iterator * const _input;
	RowCount _consumed;
	RowCount _produced;
	Record *_last;
}; // class VerifyOrderIterator
