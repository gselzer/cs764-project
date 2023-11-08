#pragma once
#include "defs.h"
#include "Record.h"

class Tree
{
public:
    virtual Record* next() = 0;
}; // class LoserTree
