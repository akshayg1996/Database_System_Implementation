#include "HeapDBFile.h"
#include <iostream>

using namespace std;

HeapDBFile::HeapDBFile()
{
}

HeapDBFile::~HeapDBFile()
{
}

// Adds a record to the file
int HeapDBFile::Add(Record &rec)
{
    if (!writePage.Append(&rec))
    {
        int pos = file.GetLength() == 0 ? 0 : file.GetLength() - 1;
        file.AddPage(&writePage, pos);
        writePage.EmptyItOut();
        writePage.Append(&rec);
    }
    return 1;
}

