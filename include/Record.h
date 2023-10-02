
#include "defs.h"
#pragma once
class Record {
public:
    int key;
    int value;
    ~Record();
   Record(int k, int v);
};
