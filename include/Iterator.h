#pragma once
#include "defs.h"
#include "Record.h"

typedef uint64_t RowCount;

class Plan
{
	friend class Iterator;
public:
	Plan ();
	virtual ~Plan ();
	virtual class Iterator * init () const = 0;
private:
}; // class Plan




class Iterator
{
public:
	Iterator ();
	virtual ~Iterator ();

	//Do we need the above 2 lines?

	void run ();
	virtual Record* next () = 0;
private:
	RowCount _count;
}; // class Iterator
