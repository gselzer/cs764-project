#include <string>
#include "defs.h"

class Record {
public:
    Record(int value,  const std::string& text);
    ~Record();
    void display() const;  // New method for displaying the record
    bool isEqual(const Record& other) const;  // New method for comparing records
    int const _value;
    std::string _text;  // New field

};