
#include "Iterator.h"
# include "Record.h"
#include <vector>

// Class that holds state used by two paired VerifyContentPlans
class VerifyContentState
{
public:
	VerifyContentState(size_t const recordSize);
	~VerifyContentState ();
	void incrementDiff(Record *r);
	void decrementDiff(Record *r);
	bool allSeen();
private:
	Record * _noDiff;
	Record * _diff;
	RowCount _diffSeen;
}; // class VerifyContentState

// Plan responsible for verifying the content of data before/after sorting
class VerifyContentPlan : public Plan
{
	friend class VerifyContentIterator;
public:
	VerifyContentPlan (Plan * const input, VerifyContentState *state, bool is_producer);
	~VerifyContentPlan ();
	Iterator * init () const;
private:
	Plan * _input;
	VerifyContentState * _state;
	const bool _isProducer;
}; // class VerifyContentPlan

// Iterator responsible for verifying the content of data before/after sorting
class VerifyContentIterator : public Iterator
{
public:
	VerifyContentIterator (VerifyContentPlan const * const plan);
	~VerifyContentIterator ();
	Record* next();
private:
	VerifyContentPlan const * const _plan;
	Iterator * const _input;
	RowCount _consumed, _produced;
	const bool _isProducer;
}; // class VerifyContentProducerIterator