
#include "defs.h"
#include <ostream>
#pragma once
class Record {
public:
    char* row1; 
    char* row2; 
    char* row3;
    int s;
    Record(Record *other);
    Record(int r1, int r2, int r3, int s);
    ~Record();
    void encodeOVC(Record *other);
    bool leOVC(Record *other);

    Record &operator= (const Record &that);
    bool operator<= (Record that);
    bool operator== (Record that);
    void operator^= (Record that);
    friend std::ostream& operator<<(std::ostream& os, const Record& obj);
private:
    int _offset, _value;
};

