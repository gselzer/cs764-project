#include "Record.h"
#include <iostream>
#include <cstring>
#include <ostream>

Record::Record() {}

Record::Record(Record &other) {
	try {
		columnSize = other.columnSize;
		
		col1 = new char[columnSize];
		col2 = new char[columnSize];
		col3 = new char[columnSize];

		memcpy(col1, other.col1, columnSize);
		memcpy(col2, other.col2, columnSize);
		memcpy(col3, other.col3, columnSize);
		_allocated = true;

		_offset = other._offset;
		_value = other._value;
		TRACE(false);
	} catch (const std::exception& e) {
        std::cerr << "Exception occurred in Record copy constructor: " << e.what() << std::endl;
        throw;
    }
}

// Constructor
Record::Record(int s) : _offset(-1), _value(-1) {
	// Divide remaining size by 3
	// TODO - don't lose remainder!
	columnSize = (s - sizeof(Record)) / 3 ;

	col1 = new char[columnSize];
	col2 = new char[columnSize];
	col3 = new char[columnSize];
	_allocated = true;

	for(int i = 0; i < columnSize - 1; i++) {
		col1[i] = (char) (rand() % 128);
		col2[i] = (char) (rand() % 128);
		col3[i] = (char) (rand() % 128);
	}
	col1[columnSize - 1] = '\0';
	col2[columnSize - 1] = '\0';
	col3[columnSize - 1] = '\0';

    TRACE(false);
}

// Constructorc
Record::Record(char *col1, char *col2, char *col3, int s) : col1(col1), col2(col2), col3(col3), _offset(-1), _value(-1) {
	// Divide remaining size by 3
	// TODO - don't lose remainder!
	columnSize = (s - sizeof(Record)) / 3 ;

    TRACE(false);
}

// Destructor
Record::~Record() {
	if (_allocated) {
		delete[] col1;
		delete[] col2;
		delete[] col3;
	}
	TRACE(false);
}

int Record::size() {
	return 3 * columnSize + sizeof(Record);
}

bool Record::operator<=(Record& that) {
	if (columnSize == 0) columnSize = that.columnSize;
	// Could maybe be written more simply with conditionals, but conditions are slow supposedly?
	for(int i = 0; i < columnSize; i++) {
		if (col1[i] != that.col1[i]) {
			return col1[i] < that.col1[i];
		}
	}
	for(int i = 0; i < columnSize; i++) {
		if (col2[i] != that.col2[i]) {
			return col2[i] < that.col2[i];
		}
	}
	for(int i = 0; i < columnSize; i++) {
		if (col3[i] != that.col3[i]) {
			return col3[i] < that.col3[i];
		}
	}
	return true;
}

Record &Record::operator=(const Record &that) {
	std::memcpy(col1, that.col1, columnSize);
	std::memcpy(col2, that.col2, columnSize);
	std::memcpy(col3, that.col3, columnSize);

	this->columnSize = that.columnSize;
	this->_offset = that._offset;
	this->_value = that._value;

	return *this;
}

bool Record::operator==(Record& that) {
	for(int i = 0; i < columnSize; i++) {
		if (col1[i] != that.col1[i]) {
			return false;
		}
	}
	for(int i = 0; i < columnSize; i++) {
		if (col2[i] != that.col2[i]) {
			return false;
		}
	}
	for(int i = 0; i < columnSize; i++) {
		if (col3[i] != that.col3[i]) {
			return false;
		}
	}
	return true;
}

void Record::operator^=(Record &that) {
	for (int i = 0; i < columnSize; i++){
		col1[i] ^= that.col1[i];
		col2[i] ^= that.col2[i];
		col3[i] ^= that.col3[i];
	}
}

std::ostream& operator<<(std::ostream& os, Record const &r) {
	os << "Record: [" << r.col1 << ", " << r.col2 << ", " << r.col3 << "] OVC: (" << r._offset << ", " << r._value << ")";
	return os;
}

void Record::encodeOVC(Record *other) {
	try {
		// Assume that other < this
		if (other == nullptr) {
			_offset = 0;
			_value = 0;
			return;
		}
		for (int i = 0; i < columnSize; i++){
			if (other->col1[i] < col1[i]) {
				_offset = i + 1;
				_value = col1[i];
				return;
			}
		}
		for (int i = 0; i < columnSize; i++){
			if (other->col2[i] < col2[i]) {
				_offset = columnSize + i + 1;
				_value = col2[i];
				return;
			}
		}
		for (int i = 0; i < columnSize; i++){
			if (other->col3[i] < col3[i]) {
				_offset = columnSize + columnSize + i + 1;
				_value = col3[i];
				return;
			}
		}
		// Equal records
		_offset = 3 * columnSize + 1;
		_value = 0;
	} catch (const std::exception& e) {
        std::cerr << "Exception occurred in Record encodeOVC(): " << e.what() << std::endl;
        throw;
    }
}

bool Record::leOVC(Record *other) {
	try {
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
	} catch (const std::exception& e) {
        std::cerr << "Exception occurred in Record leOVC(): " << e.what() << std::endl;
        throw;
    }
}

