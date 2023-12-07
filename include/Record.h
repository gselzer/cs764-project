
#include "defs.h"
#include <ostream>
#pragma once
class Record {
public:
    Record();
    Record(Record &other);
    Record(int s);
    Record(char *col1, char *col2, char *col3, int s);
    ~Record();
    void encodeOVC(Record *other);
    bool leOVC(Record *other);
    int size();
    Record &operator= (const Record &that);
    bool operator<= (Record& that);
    bool operator== (Record& that);
    void operator^= (Record& that);
    friend std::ostream& operator<<(std::ostream& os, const Record& obj);

    char* col1; 
    char* col2; 
    char* col3;
    int columnSize;
    int _offset, _value;
    bool _allocated = false;
};

