
#include "defs.h"
#include <ostream>
#pragma once
class Record {
public:
    char *row1, *row2, *row3;
    Record(Record *other);
    Record(char *r1, char *r2, char *r3);
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
