
#include "defs.h"
#include <ostream>
#pragma once
class Record {
public:
    char* row1; 
    char* row2; 
    char* row3;
    Record(Record *other);
    Record(size_t s);
    Record(char *row1, char *row2, char *row3, size_t s);
    ~Record();
    void encodeOVC(Record *other);
    bool leOVC(Record *other);
    size_t size();

    Record &operator= (const Record &that);
    bool operator<= (Record& that);
    bool operator== (Record& that);
    void operator^= (Record& that);
    friend std::ostream& operator<<(std::ostream& os, const Record& obj);
    size_t rowSize;
private:
    int _offset, _value;
    bool _allocated = false;
};

