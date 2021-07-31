#ifndef BIGQ_H
#define BIGQ_H
#pragma once

#include <pthread.h>
#include <iostream>
#include "Pipe.h"
#include "File.h"
#include "Record.h"
#include <vector>
#include <stdio.h>
#include "ComparisonEngine.h"
#include <algorithm>
#include "DBFile.h"
#include <string>
#include <queue>
#include <string.h>
#include <fstream>

//class HeapDBFile;

using namespace std;

class BigQ
{
    Pipe &inputPipe;
    Pipe &outputPipe;
    pthread_t threadID;
    OrderMaker &orderMaker;
    int runLength;
    string tempFile;
    File file;
    int noOfRecordsPerPage;

    void SetNoOfRecordsPerPage(const char *fpath)
    {

        std::string fileName(strdup(fpath));

        if(fileName == "bin/nation.bin")
            this->noOfRecordsPerPage = 25; // nation.tbl
        else if(fileName == "bin/region.bin")
            this->noOfRecordsPerPage = 5; // region.tbl
        else if(fileName == "bin/customer.bin")
            this->noOfRecordsPerPage = 643; // customer.tbl
        else if(fileName == "bin/part.bin")
            this->noOfRecordsPerPage = 769; // part.tbl
        else if(fileName == "bin/partsupp.bin")
            this->noOfRecordsPerPage = 748; // partsupp.tbl
        else if(fileName == "bin/supplier.bin")
            this->noOfRecordsPerPage = 100; // supplier.tbl
        else if(fileName == "bin/orders.bin")
            this->noOfRecordsPerPage = 830; // orders.tbl
        else if(fileName == "bin/lineitem.bin")
            this->noOfRecordsPerPage = 606; // lineitem.tbl

    }

public:

    BigQ(Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen, const char *fpath);

    ~BigQ();

    int OpenFile(string fileName);

    int CloseFile();

//	bool CompareRecords(const Record *p, const Record *q);
    //bool ComparePairOfRecords(pair<int, Record *> p, pair<int, Record *> q);
    static void *SortAndDumpToPipe(void *bigQ);
};

#endif
