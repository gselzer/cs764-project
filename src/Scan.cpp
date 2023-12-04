#include "Scan.h"
#include "../include/Record.h"
#include<iostream>
#include<cstdlib>

ScanPlan::ScanPlan (RowCount const count, size_t const recordSize) : _count (count), _recordSize(recordSize)
{
	TRACE (false);
} // ScanPlan::ScanPlan

ScanPlan::~ScanPlan ()
{
	TRACE (false);
} // ScanPlan::~ScanPlan

Iterator * ScanPlan::init () const
{
	TRACE (false);
	return new ScanIterator (this);
} // ScanPlan::init

ScanIterator::ScanIterator (ScanPlan const * const plan) :
	_plan (plan), _count (0), _recordSize(plan->_recordSize)
{
	TRACE (false);
	// TODO: Consider random initialization
	// srand((unsigned) time(NULL));

} // ScanIterator::ScanIterator

ScanIterator::~ScanIterator ()
{
	TRACE (false);
	traceprintf ("produced %lu of %lu rows\n",
			(unsigned long) (_count),
			(unsigned long) (_plan->_count));
} // ScanIterator::~ScanIterator

Record* ScanIterator::next ()
{
    TRACE (false);
    if (_count >= _plan->_count) {
        return nullptr; // Stopping condition
    }
    _count++;
    Record * r = new Record(_recordSize);
	// int s;
	// for (int i = 0; i < s / 3; i++){
	// if (r->row1[i] > 100 || r->row2[i] > 100 || r->row3[i] > 100) {
	// 	std::cout << "Erroneous row: " << *r << "\n";
	// }
	// }
	return r;
}// ScanIterator::next
