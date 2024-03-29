#include "Filter.h"

#include<cstdlib>

// NB We didn't implement any extra credit here, and don't call any of this in the sort
FilterPlan::FilterPlan (Plan * const input) : _input (input)
{
	TRACE (true);
} // FilterPlan::FilterPlan

FilterPlan::~FilterPlan ()
{
	TRACE (true);
	delete _input;
} // FilterPlan::~FilterPlan

Iterator * FilterPlan::init () const
{
	TRACE (true);
	return new FilterIterator (this);
} // FilterPlan::init

FilterIterator::FilterIterator (FilterPlan const * const plan) :
	_plan (plan), _input (plan->_input->init ()),
	_consumed (0), _produced (0)
{
	TRACE (true);
} // FilterIterator::FilterIterator

FilterIterator::~FilterIterator ()
{
	TRACE (true);

	delete _input;

	traceprintf ("produced %lu of %lu rows\n",
			(unsigned long) (_produced),
			(unsigned long) (_consumed));
} // FilterIterator::~FilterIterator

Record* FilterIterator::next ()
{
	TRACE (true);
	Record *r;
	do
	{
		r = _input->next();
		if (r == nullptr)  return NULL;
		++ _consumed;
		if (_consumed % 2 != 0) free(r);
	} while (_consumed % 2 == 0);

	++ _produced;
	return r;
} // FilterIterator::next
