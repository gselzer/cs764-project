#include "VerifyContent.h"
#include<cassert>
#include<cstdlib>
#include<limits>
#include<iostream>

VerifyContentState::VerifyContentState() : _diffSeen(0)
{
	_diff = new Record(0, 0, 0);
	_noDiff = new Record(0, 0, 0);
}

VerifyContentState::~VerifyContentState()
{
	delete _diff;
	delete _noDiff;
}

void VerifyContentState::incrementDiff(Record *r) {
	*_diff ^= *r;
	_diffSeen++;
}

void VerifyContentState::decrementDiff(Record *r) {
	*_diff ^= *r;
	_diffSeen--;
}

bool VerifyContentState::allSeen() {
	bool allSeen = *_diff == *_noDiff && _diffSeen == 0;
	return allSeen;
}


VerifyContentPlan::VerifyContentPlan (Plan *const input, VerifyContentState *const state, bool isProducer) : _input (input), _state (state), _isProducer(isProducer)
{
} // VerifyContentPlan::VerifyContentPlan

VerifyContentPlan::~VerifyContentPlan ()
{
} // VerifyContentPlan::~VerifyContentPlan

Iterator * VerifyContentPlan::init () const
{
	return new VerifyContentIterator (this);
} // VerifyContentPlan::init

VerifyContentIterator::VerifyContentIterator (VerifyContentPlan const * const plan) :
	_plan (plan),
	_input (_plan->_input->init()),
	_consumed(0),
	_produced(0),
	_isProducer(_plan->_isProducer)
{
} // VerifyContentIterator::VerifyContentIterator

VerifyContentIterator::~VerifyContentIterator ()
{
	std::cout<< ("Verifying Content: produced %lu of %lu rows\n",
			(unsigned long) (_produced),
			(unsigned long) (_consumed));

	assert (_plan->_state->allSeen());

	delete _input;
} // VerifyContentIterator::~VerifyContentIterator

Record* VerifyContentIterator::next ()
{
	Record *r = _input->next();
	if (r != nullptr) {
		_consumed++;
		_produced++;
		if (_isProducer)
			_plan->_state->incrementDiff(r);
		else
			_plan->_state->decrementDiff(r);
	}
	return r;
}// VerifyContentIterator::next
