
#include "defs.h"
#include <ostream>
#pragma once
class Record {
public:
    char* row1; 
    char* row2; 
    char* row3;
    Record();
    Record(Record &other);
    Record(int s);
    Record(char *row1, char *row2, char *row3, int s);
    ~Record();
    void encodeOVC(Record *other);
    bool leOVC(Record *other);
    int size();

    Record &operator= (const Record &that);
    bool operator<= (Record& that);
    bool operator== (Record& that);
    void operator^= (Record& that);
    friend std::ostream& operator<<(std::ostream& os, const Record& obj);
    int rowSize;
    int _offset, _value;
    bool _allocated = false;
};

