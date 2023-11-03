#include "Record.h"
// Constructor
Record::Record(int r1, int r2, int r3) : row1(r1), row2(r2), row3(r3) {
	TRACE(false);
}// ScanPlan::ScanPlan

// Destructor
Record::~Record() {
	TRACE(false);
}// ScanPlan::~ScanPlan

bool Record::operator<=(Record that) {
	// Could maybe be written more simply with conditionals, but conditions are slow supposedly?
	return (
		(this->row1 < that.row1) ||
		(this->row1 == that.row1 && this->row2 < that.row2) ||
		(this->row1 == that.row1 && this->row2 == that.row2 && this->row3 < that.row3)
	);
};

bool Record::operator==(Record that) {
	return (
		(this->row1 == that.row1) &&
		(this->row2 == that.row2) &&
		(this->row3 == that.row3)
	);
};

void Record::operator^=(Record that) {
	row1 ^= that.row1;
	row2 ^= that.row2;
	row3 ^= that.row3;
};
