#include "Record.h"
#include <iostream>
#include <cstring>
#include <ostream>

Record::Record(Record &other) {
	 
	rowSize = other.rowSize;
	
	row1 = new char[rowSize];
	row2 = new char[rowSize];
	row3 = new char[rowSize];

	memcpy(row1, other.row1, rowSize);
	memcpy(row2, other.row2, rowSize);
	memcpy(row3, other.row3, rowSize);
	_allocated = true;

    _offset = other._offset;
    _value = other._value;
	TRACE(false);
}

// Constructor
Record::Record(int s) : _offset(-1), _value(-1) {
	// Divide remaining size by 3
	// TODO - don't lose remainder!
	rowSize = (s - sizeof(Record)) / 3 ;

	row1 = new char[rowSize];
	row2 = new char[rowSize];
	row3 = new char[rowSize];
	_allocated = true;

	for(int i = 0; i < rowSize - 1; i++) {
		row1[i] = (char) (rand() % 128);
		row2[i] = (char) (rand() % 128);
		row3[i] = (char) (rand() % 128);
	}
	row1[rowSize - 1] = '\0';
	row2[rowSize - 1] = '\0';
	row3[rowSize - 1] = '\0';

    TRACE(false);
}

// Constructorc
Record::Record(char *row1, char *row2, char *row3, int s) : row1(row1), row2(row2), row3(row3), _offset(-1), _value(-1) {
	// Divide remaining size by 3
	// TODO - don't lose remainder!
	rowSize = (s - sizeof(Record)) / 3 ;

    TRACE(false);
}

// Destructor
Record::~Record() {
	if (_allocated) {
		delete row1;
		delete row2;
		delete row3;
	}
	TRACE(false);
}

int Record::size() {
	return 3 * rowSize + sizeof(Record);
}

bool Record::operator<=(Record& that) {
	if (rowSize == 0) rowSize = that.rowSize;
	// Could maybe be written more simply with conditionals, but conditions are slow supposedly?
	for(int i = 0; i < rowSize; i++) {
		if (row1[i] != that.row1[i]) {
			return row1[i] < that.row1[i];
		}
	}
	for(int i = 0; i < rowSize; i++) {
		if (row2[i] != that.row2[i]) {
			return row2[i] < that.row2[i];
		}
	}
	for(int i = 0; i < rowSize; i++) {
		if (row3[i] != that.row3[i]) {
			return row3[i] < that.row3[i];
		}
	}
	return true;
}

Record &Record::operator=(const Record &that) {
	std::memcpy(row1, that.row1, rowSize);
	std::memcpy(row2, that.row2, rowSize);
	std::memcpy(row3, that.row3, rowSize);

	this->rowSize = that.rowSize;
	this->_offset = that._offset;
	this->_value = that._value;

	return *this;
}

bool Record::operator==(Record& that) {
	for(int i = 0; i < rowSize; i++) {
		if (row1[i] != that.row1[i]) {
			return false;
		}
	}
	for(int i = 0; i < rowSize; i++) {
		if (row2[i] != that.row2[i]) {
			return false;
		}
	}
	for(int i = 0; i < rowSize; i++) {
		if (row3[i] != that.row3[i]) {
			return false;
		}
	}
	return true;
}

void Record::operator^=(Record &that) {
	for (int i = 0; i < rowSize; i++){
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
	if (other == nullptr) {
		_offset = 0;
		_value = 0;
		return;
	}
	for (int i = 0; i < rowSize; i++){
		if (other->row1[i] < row1[i]) {
			_offset = i + 1;
			_value = row1[i];
			return;
		}
	}
	for (int i = 0; i < rowSize; i++){
		if (other->row2[i] < row2[i]) {
			_offset = rowSize + i + 1;
			_value = row2[i];
			return;
		}
	}
	for (int i = 0; i < rowSize; i++){
		if (other->row3[i] < row3[i]) {
			_offset = rowSize + rowSize + i + 1;
			_value = row3[i];
			return;
		}
	}
	// Equal records
	_offset = 3 * rowSize + 1;
	_value = 0;
}

bool Record::leOVC(Record *other) {
	if(other->_offset == 0 && other->_value == 0 || _offset == 0 && _value == 0) {
		return *this <= *other;
	}
	if (_offset != other->_offset) {
		return _offset > other->_offset;
	}
	if (_value != other->_value) {
		return _value < other->_value;
	}
	return *this <= *other;
}

