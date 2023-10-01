#include "Record.h"
// Constructor
Record::Record(int k, int v) : key(k), value(v) {
	TRACE(true);
}// ScanPlan::ScanPlan

// Destructor
Record::~Record() {
	TRACE(true);
}// ScanPlan::~ScanPlan
