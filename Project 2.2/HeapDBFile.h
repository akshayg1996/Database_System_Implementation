#pragma once

#include "GenericDBFile.h"
#include "Record.h"

class HeapDBFile : public GenericDBFile
{
public:
    HeapDBFile();
    ~HeapDBFile();
    int Add(Record &addme) override;
};