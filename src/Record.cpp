#include "Record.h"
// Constructor
Record::Record(int r1, int r2, int r3) : row1(r1), row2(r2), row3(r3) {
	TRACE(true);
}// ScanPlan::ScanPlan

// Destructor
Record::~Record() {
	TRACE(true);
}// ScanPlan::~ScanPlan

bool Record::operator<=(Record *const that) {
	return this->row1 <= that->row1;
};
