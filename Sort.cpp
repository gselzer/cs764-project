#include "Sort.h"
#include <cstdlib>

SortPlan::SortPlan (Plan * const input) : _input (input)
{
	TRACE (true);
} // SortPlan::SortPlan

SortPlan::~SortPlan ()
{
	TRACE (true);
	delete _input;
} // SortPlan::~SortPlan

Iterator * SortPlan::init () const
{
	TRACE (true);
	return new SortIterator (this);
} // SortPlan::init

SortIterator::SortIterator (SortPlan const * const plan) :
	_plan (plan), _input (plan->_input->init ()),
	_consumed (0), _produced (0)
{
	TRACE (true);

	Record *r = _input->next();
	while (r) {
		// TODO
		++ _consumed;
		free(r);
		r = _input->next();
	}  
	delete _input;

	traceprintf ("consumed %lu rows\n",
			(unsigned long) (_consumed));
} // SortIterator::SortIterator

SortIterator::~SortIterator ()
{
	TRACE (true);

	traceprintf ("produced %lu of %lu rows\n",
			(unsigned long) (_produced),
			(unsigned long) (_consumed));
} // SortIterator::~SortIterator

Record* SortIterator::next ()
{
	TRACE (true);

	if (_produced >= _consumed)  return NULL;

	++ _produced;
	// TODO
	return NULL;
} // SortIterator::next
