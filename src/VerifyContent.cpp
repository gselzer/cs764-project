#include "VerifyContent.h"
#include<cassert>
#include<cstdlib>
#include<limits>
#include<iostream>

// Keeps track of all records seen
// Note that we use an XOR function to check content before and after in an efficient manner,
// as there is a nearly impossible chance for us to see different records before and after
// and for the xor to be zero.
VerifyContentState::VerifyContentState(size_t const recordSize) : _diffSeen(0)
{
	_diff = new Record(recordSize);
	_noDiff = new Record(recordSize);
	for(int i = 0; i < _diff->columnSize; i++) {
		_diff->col1[i] = 0;
		_diff->col2[i] = 0;
		_diff->col3[i] = 0;
		_noDiff->col1[i] = 0;
		_noDiff->col2[i] = 0;
		_noDiff->col3[i] = 0;
	}
}

VerifyContentState::~VerifyContentState()
{
	delete _diff;
	delete _noDiff;
}

// XOR the record as it enters
void VerifyContentState::incrementDiff(Record *r) {
	*_diff ^= *r;
	_diffSeen++;
}

// XOR the record as it leaves
void VerifyContentState::decrementDiff(Record *r) {
	*_diff ^= *r;
	_diffSeen--;
}

// Assert that all records that entered have also left
bool VerifyContentState::allSeen() {
	bool allSeen = *_diff == *_noDiff && _diffSeen == 0;
	return allSeen;
}

// The plan works either as a producer or as a consumer
// Producer plans increment the xor difference as they see records.
// Consumer plans decrement the xor difference as they see records.
// Consumer plans are also responsible for asserting that all records
// it saw were also seen by the producer when it is destroyed.
VerifyContentPlan::VerifyContentPlan (Plan *const input, VerifyContentState *const state, bool isProducer) : _input (input), _state (state), _isProducer(isProducer)
{
} // VerifyContentPlan::VerifyContentPlan

VerifyContentPlan::~VerifyContentPlan ()
{
	if(!_isProducer) {
		delete _state;
	}
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
	if(!_isProducer){
	assert (_plan->_state->allSeen());
std::cout<< " Verifying Content: produced "<<(unsigned long) (_produced)<< " of "<< (unsigned long) (_consumed)<<" rows\n";
	}
	
	else{
	assert (_plan->_state->allSeen());
std::cout<< " Verifying Content: Consumed "<<(unsigned long) (_produced)<< " of "<< (unsigned long) (_consumed)<<" rows\n";
	}
		

	
	delete _input;
} // VerifyContentIterator::~VerifyContentIterator

Record* VerifyContentIterator::next ()
{
	Record *r = _input->next();
	if (r != nullptr) {
		_consumed++;
		_produced++;
		// Change the diff, depending on whether this Iterator is a producer or consumer.
		if (_isProducer)
			_plan->_state->incrementDiff(r);
		else
			_plan->_state->decrementDiff(r);
	}
	return r;
}// VerifyContentIterator::next
