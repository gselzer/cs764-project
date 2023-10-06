#include "VerifyOrder.h"
#include<cassert>
#include<cstdlib>
#include<limits>

VerifyOrderPlan::VerifyOrderPlan (Plan *const input) : _input (input)
{
	TRACE (true);
} // VerifyOrderPlan::VerifyOrderPlan

VerifyOrderPlan::~VerifyOrderPlan ()
{
	TRACE (true);
} // VerifyOrderPlan::~VerifyOrderPlan

Iterator * VerifyOrderPlan::init () const
{
	TRACE (true);
	return new VerifyOrderIterator (this);
} // VerifyOrderPlan::init

VerifyOrderIterator::VerifyOrderIterator (VerifyOrderPlan const * const plan) :
	_plan (plan),
	_input (_plan->_input->init()),
	_lastValue(std::numeric_limits<int>::min()) // TODO: Consider types other than ints
{
	TRACE (true);

} // VerifyOrderIterator::VerifyOrderIterator

VerifyOrderIterator::~VerifyOrderIterator ()
{
	TRACE (true);
	traceprintf ("produced %lu of %lu rows\n",
			(unsigned long) (_produced),
			(unsigned long) (_consumed));
} // VerifyOrderIterator::~VerifyOrderIterator

Record* VerifyOrderIterator::next ()
{
    TRACE (true);
	Record *r = _input->next();
	if (r != nullptr) {
		assert(r->row1 >= _lastValue);
		_lastValue = r->row1;
	}
	return r;
}// VerifyOrderIterator::next
