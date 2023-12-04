#include "Record.h"
#include <iostream>
#include <ostream>

int s;
Record::Record(Record *other) {
	 if (other == nullptr) {
        throw std::invalid_argument("Null pointer passed to Record copy constructor");
    }
	
	row1 = (char *) malloc((s / 3) * sizeof(char));
	row2 = (char *) malloc((s / 3) * sizeof(char));
	row3 = (char *) malloc((s / 3) * sizeof(char));

    _offset = other->_offset;
    _value = other->_value;
	TRACE(false);
}

// Constructor
Record::Record(int r1, int r2, int r3, int s) : row1(r1), row2(r2), row3(r3), _offset(-1), _value(-1) {
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

	return *this;
}

bool Record::operator==(Record that) {
	return (
		(this->row1 == that.row1) &&
		(this->row2 == that.row2) &&
		(this->row3 == that.row3)
	);
}

void Record::operator^=(Record that) {
	int s;
	for (int i = 0; i < s / 3; i++){
		row1[i] ^= that.row1[i];
		row2[i] ^= that.row2[i];
		row3[i] ^= that.row3[i];
	}
	
}

std::ostream& operator<<(std::ostream& os, Record const &r) {
	os << "Record: [" << r.row1 << ", " << r.row2 << ", " << r.row3 << "] OVC: (" << r._offset << ", " << r._value << ")";
	return os;
}

void Record::encodeOVC(Record *other) {
	// Assume that other < this
	int s;
	for (int i = 0; i < s / 3; i++){

	if (other == nullptr) {
		_offset = 0;
		_value = row1[i];
		return;
	}

	if (other->row1[i] < row1[i]) {
		_offset = 1;
		_value = row1[i];
	} else if (other->row2[i] < row2[i]) {
		_offset = 2;
		_value = row2[i];
	} else if (other->row3[i] < row3[i]) {
		_offset = 3;
		_value = row3[i];
	} else {
		// Equal records
		_offset = 4;
		_value = row1[i];
	}
	}
}

bool Record::leOVC(Record *other) {
	if (_offset != other->_offset) {
		return _offset > other->_offset;
	}
	if (_value != other->_value) {
		return _value < other->_value;
	}
	return *this <= other;
}

