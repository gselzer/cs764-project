#include "Scan.h"
#include "../include/Record.h"
#include<iostream>
#include<cstdlib>
#include <stdexcept>

ScanPlan::ScanPlan (RowCount const count, size_t const recordSize) : _count (count), recordSize(recordSize)
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
	_plan (plan), _count (0), recordSize(plan->recordSize)
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

// Generates a new random record
Record* ScanIterator::next ()
{
    TRACE (false);
	try
    {
		if (_count >= _plan->_count) {
			return nullptr; // Stopping condition
		}
		_count++;
		Record * r = new Record(recordSize);
		return r;
	}
	catch (const std::exception& e)
	{
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        throw;
    }
}// ScanIterator::next
