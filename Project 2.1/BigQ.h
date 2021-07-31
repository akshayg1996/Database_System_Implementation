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

class DBFile;

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
        cout << "Rel path = " << std::string(fpath) << endl;
        DBFile readFile;
        DBFile writeFile;
        readFile.Open(fpath);
        writeFile.Create("writePath.bin", heap, NULL);
        readFile.MoveFirst();
        Record record;
        int cnt = 0;
        bool flag = false;
        while (readFile.GetNext(record) && ++cnt)
        {
            //cout << "cnt = " << cnt << endl;
            //cout << "dbfile.GetLength() = " << readFile.GetLength() << endl;
            //cout << "No of records per page = " << this->noOfRecordsPerPage << endl;
            writeFile.Add(record);
            if (writeFile.GetLength() == 2)
            {
                this->noOfRecordsPerPage = cnt - 1;
                flag = true;
                cout << "No of records per page = " << this->noOfRecordsPerPage << endl;
                break;
            }
        }

        if (cnt != 0 && !flag)
        {
            this->noOfRecordsPerPage = cnt;
            cout << "No of records per page = " << this->noOfRecordsPerPage << endl;
        }

        readFile.Close();
        writeFile.Close();
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
