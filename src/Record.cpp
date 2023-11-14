#include "Record.h"
#include <iostream>
#include <ostream>

Record::Record(Record *other) : row1(other->row1), row2(other->row2), row3(other->row3), _offset(other->_offset), _value(other->_value) {
	TRACE(false);
}

// Constructor
Record::Record(int r1, int r2, int r3) : row1(r1), row2(r2), row3(r3), _offset(-1), _value(-1) {
	TRACE(false);
}

// Destructor
Record::~Record() {
	TRACE(false);
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

	this->_offset = that._offset;
	this->_value = that._value;
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
	os << "Record: [" << r.row1 << ", " << r.row2 << ", " << r.row3 << "] OVC: (" << r._offset << ", " << r._value << ")";
	return os;
}

void Record::encodeOVC(Record *other) {
	// Assume that other < this
	if (other == nullptr) {
		_offset = 0;
		_value = row1;
		return;
	}

	if (other->row1 < row1) {
		_offset = 1;
		_value = row1;
	} else if (other->row2 < row2) {
		_offset = 2;
		_value = row2;
	} else if (other->row3 < row3) {
		_offset = 3;
		_value = row3;
	} else {
		// Equal records
		_offset = 0;
		_value = row1;
	}

}

bool Record::leOVC(Record *other) {
	if (_offset != other->_offset) {
		return _offset < other->_offset;
	}
	return _value <= other->_value;
}

