#include "Iterator.h"
#include "LoserTree.h"
#include "Record.h"
#include "Run.h"
#include <vector>

class ExternalMergeSortPlan : public Plan
{
public:
    ExternalMergeSortPlan(Plan* input);
    ~ExternalMergeSortPlan();
    Iterator* init() const;

private:
    Plan* _input;
};

class ExternalMergeSortIterator : public Iterator
{
public:
    ExternalMergeSortIterator(const ExternalMergeSortPlan* plan);
    ~ExternalMergeSortIterator();
    Record* next();

private:
    void sortDramAndStore(uint64_t totalDataSize, uint64_t dramOffset, bool isSsdSorted, const std::string& outputHdd);
    void sortSsdAndStore(uint64_t totalDataSize, uint64_t ssdOffset, bool isDramSorted, const std::string& outputHdd);
    void sortHddAndStore(uint64_t totalDataSize, uint64_t hddOffset, const std::string& outputHdd);
    void mergeAndStoreRuns(const std::vector<Run*>& runs, const std::string& outputHdd);
    void readDataFromHDD(uint64_t offset, std::vector<char>& data);
    void readDataFromSSD(uint64_t offset, std::vector<char>& data);
    void writeDataToHDD(uint64_t offset, const std::vector<char>& data, const std::string& outputHdd);

    const ExternalMergeSortPlan* _plan;
    Iterator* _input;
    size_t _currentIdx;
    RunStorageState* _state;
    MultiStageLoserTree* _tree;
    std::vector<Record*> _dramRecords;
    std::vector<Record*> _ssdRecords;
};
