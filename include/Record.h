
#include "defs.h"
#include <ostream>
#pragma once
class Record {
public:
    int row1, row2, row3;
    ~Record();
    Record(int r1, int r2, int r3);
    bool operator<= (Record that);
    bool operator== (Record that);
    void operator^= (Record that);
    friend std::ostream& operator<<(std::ostream& os, const Record& obj);
};
