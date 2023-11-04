#include "Record.h"
#include <ostream>

Record::Record(Record *other) : row1(other->row1), row2(other->row2), row3(other->row3) {
	TRACE(false);
}

// Constructor
Record::Record(int r1, int r2, int r3) : row1(r1), row2(r2), row3(r3) {
	TRACE(false);
}

// Destructor
Record::~Record() {
	TRACE(false);
}


void Record::set(Record *other) {
	this->row1 = other->row1;
	this->row2 = other->row2;
	this->row3 = other->row3;
}

bool Record::operator<=(Record that) {
	// Could maybe be written more simply with conditionals, but conditions are slow supposedly?
	return (
		(this->row1 < that.row1) ||
		(this->row1 == that.row1 && this->row2 < that.row2) ||
		(this->row1 == that.row1 && this->row2 == that.row2 && this->row3 <= that.row3)
	);
}

Record &Record::operator=(const Record &that) {
	this->row1 = that.row1;
	this->row2 = that.row2;
	this->row3 = that.row3;
}

bool Record::operator==(Record that) {
	return (
		(this->row1 == that.row1) &&
		(this->row2 == that.row2) &&
		(this->row3 == that.row3)
	);
}

void Record::operator^=(Record that) {
	row1 ^= that.row1;
	row2 ^= that.row2;
	row3 ^= that.row3;
}

std::ostream& operator<<(std::ostream& os, Record const &r) {
	os << "Record: [" << r.row1 << ", " << r.row2 << ", " << r.row3 << "]";
	return os;
}

