#include "VerifyOrder.h"
#include<cassert>
#include<cstdlib>
#include<limits>
#include<iostream>

VerifyOrderPlan::VerifyOrderPlan (Plan *const input, size_t const recordSize) : _input (input), recordSize(recordSize)
{
	TRACE (false);
} // VerifyOrderPlan::VerifyOrderPlan

VerifyOrderPlan::~VerifyOrderPlan ()
{
	TRACE (false);
} // VerifyOrderPlan::~VerifyOrderPlan

Iterator * VerifyOrderPlan::init () const
{
	TRACE (false);
	return new VerifyOrderIterator (this);
} // VerifyOrderPlan::init

VerifyOrderIterator::VerifyOrderIterator (VerifyOrderPlan const * const plan) :
	_plan (plan),
	_input (_plan->_input->init()),
	_consumed (0),
	_produced (0)
{
	_last = new Record(plan->recordSize);
	TRACE (false);
} // VerifyOrderIterator::VerifyOrderIterator

VerifyOrderIterator::~VerifyOrderIterator ()
{
	TRACE (false);
	std::cout<< "Verify Order: produced "<< (unsigned long) (_produced) <<" of "<< (unsigned long) (_consumed)<<" rows\n";

    delete _input;
	delete _last;
} // VerifyOrderIterator::~VerifyOrderIterator

Record* VerifyOrderIterator::next ()
{
    TRACE (false);
	Record *r = _input->next();
	if (_produced == 0) {
		std::cout << "Setting the Start Record\n";
		*_last = *r;
	}
	if (r != nullptr) {
		_produced++;
		assert(*_last <= *r);
		*_last = *r;
		_consumed++;
	}
	return r;
}// VerifyOrderIterator::next
