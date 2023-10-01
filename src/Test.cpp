#include "Iterator.h"
#include "Scan.h"
#include "Sort.h"
#include <vector>

int main() {
    ScanPlan scanPlan(100);
    Iterator* it = scanPlan.init();

    std::vector<Record*> records;
    for (int i = 0; i < 100; ++i) {
        records.push_back(it->next());
    }

    SortPlan sortPlan(&scanPlan);
    Iterator* sorter = sortPlan.init();
    dynamic_cast<SortIterator*>(sorter)->sort(records);

    // Cleanup
    delete it;
    delete sorter;

    // Delete dynamically allocated DataRecords
    for (auto record : records) {
        delete record;
    }

    return 0;
}
