#include "Scan.h"
#include<cstdlib>

ScanPlan::ScanPlan (RowCount const count) : _count (count)
{
	TRACE (true);
} // ScanPlan::ScanPlan

ScanPlan::~ScanPlan ()
{
	TRACE (true);
} // ScanPlan::~ScanPlan

Iterator * ScanPlan::init () const
{
	TRACE (true);
	return new ScanIterator (this);
} // ScanPlan::init

ScanIterator::ScanIterator (ScanPlan const * const plan) :
	_plan (plan), _count (0)
{
	TRACE (true);
	// TODO: Consider random initialization
	// srand((unsigned) time(NULL));

} // ScanIterator::ScanIterator

ScanIterator::~ScanIterator ()
{
	TRACE (true);
	traceprintf ("produced %lu of %lu rows\n",
			(unsigned long) (_count),
			(unsigned long) (_plan->_count));
} // ScanIterator::~ScanIterator

Record* ScanIterator::next ()
{
    TRACE (true);
    if (_count >= _plan->_count) {
        return nullptr; // Stopping condition
    }
    _count++;
    return new Record(
		rand() % 100, // Row 1
		rand() % 100, // Row 2
		rand() % 100  // Row 3
	);
}// ScanIterator::next
