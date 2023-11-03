#include "Record.h"
// Constructor
Record::Record(int r1, int r2, int r3) : row1(r1), row2(r2), row3(r3) {
	// TRACE(true);
}// ScanPlan::ScanPlan

// Destructor
Record::~Record() {
	// TRACE(true);
}// ScanPlan::~ScanPlan

bool Record::operator<=(Record that) {
	return this->row1 <= that.row1;
};

bool Record::operator==(Record that) {
	return (
		this->row1 == that.row1
		&& this->row2 == that.row2
		&& this->row3 == that.row3
	);
};

void Record::operator^=(Record that) {
	row1 ^= that.row1;
	row2 ^= that.row2;
	row3 ^= that.row3;
};
