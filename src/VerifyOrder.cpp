#include "VerifyOrder.h"
#include<cassert>
#include<cstdlib>
#include<limits>
#include<iostream>

VerifyOrderPlan::VerifyOrderPlan (Plan *const input) : _input (input)
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
	_lastValue(std::numeric_limits<int>::min()) // TODO: Consider types other than ints
{
	TRACE (false);

} // VerifyOrderIterator::VerifyOrderIterator

VerifyOrderIterator::~VerifyOrderIterator ()
{
	TRACE (false);
	traceprintf ("produced %lu of %lu rows\n",
			(unsigned long) (_produced),
			(unsigned long) (_consumed));

    delete _input;
} // VerifyOrderIterator::~VerifyOrderIterator

Record* VerifyOrderIterator::next ()
{
    TRACE (false);
	Record *r = _input->next();
	if (r != nullptr) {
		assert(r->row1 >= _lastValue);
		_lastValue = r->row1;
	}
	return r;
}// VerifyOrderIterator::next
