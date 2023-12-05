
#include "Iterator.h"
#include "Record.h"

class ScanPlan : public Plan
{
	friend class ScanIterator;
public:
	ScanPlan (RowCount const count, size_t const recordSize);
	~ScanPlan ();
	Iterator * init () const;
private:
	RowCount const _count;
	size_t const _recordSize;
}; // class ScanPlan

class ScanIterator : public Iterator
{
public:
	ScanIterator (ScanPlan const * const plan);
	~ScanIterator ();
	Record* next() override; // modified to return Record
private:
	ScanPlan const * const _plan;
	RowCount _count;
	size_t _recordSize;
}; // class ScanIterator
