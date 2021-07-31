#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include "string.h"
#include "iostream"
#include "fstream"

// stub file .. replace it with your own DBFile.cc

DBFile::DBFile() : databaseFile(), databasePage(), pageNumber(0) {
}

/*
    Creates the corresponding database file from the path mentioned below
    Returns 1 on successful open; 0 on failed
*/
int DBFile::Create(const char *f_path, fType f_type, void *startup)
{
    try
    {
        this->fileName = strdup(f_path);
        this->databaseFile.Open(0, this->fileName);
        cout << "File created successfully\n";
    }
    catch (...)
    {
        cerr << "Error occured while creating DBFileHeap. \n";
        return 0;
    }
    return 1;
}

// Loads the tbl file into a database file having a set of pages. Each page contains a set of records
void DBFile::Load(Schema &f_schema, const char *loadpath)
{
    // Open file in read mode
    auto *loadFile = fopen(loadpath, "r");
    try
    {
        if (loadFile == NULL)
            throw ("File:" + std::string(loadpath) + " error");

        Record record;
        int recCount = 0;

        while (record.SuckNextRecord(&f_schema, loadFile) == 1)
        {
            this->Add(record);
            recCount++;
            if (recCount % 10000 == 0)
            {
                cout << "Record Counter: " << recCount << std::endl;
                cout << "Page Counter: " << pageNumber << std::endl;
            }
        }

        //The left over records when the page is not full should be added to a new page at the end of the file.
        this->databaseFile.AddPage(&this->databasePage, pageNumber++);
        cout << "Record Counter: " << recCount % 10000 << std::endl;
        cout << "Page Counter: " << pageNumber << std::endl;

        cout << "---------------------------------------------" << std::endl;
        cout << "Total number of records added to file = " << recCount << std::endl;
        cout << "Total number of pages added to file = " << pageNumber << std::endl;
    }
    catch (exception e)
    {
        cerr << e.what();
        fclose(loadFile);
    }
}

/*
    Opens database file.
    Returns 1 on success; 0 on failure
*/
int DBFile::Open(const char *f_path)
{
    try
    {
        this->fileName = strdup(f_path);
        if(!std::ifstream(this->fileName))
        {
            cerr << "ERROR: File does not exist or could not be opened" << std::endl;
            return 0;
        }
        this->databaseFile.Open(1, this->fileName);
        cout << "File present at path: " << std::string(f_path) << " is now opened. \n";
    }
    catch (exception ex)
    {
        cerr << ex.what();
        return 0;
    }
    return 1;
}

/*
    Closes database file.
    Returns 1 on success; 0 on failure
*/
int DBFile::Close()
{
    try
    {
        ifstream file(this->fileName);
        if(file.is_open())
            this->databaseFile.Close();
        else
        {
            cerr << "In DBFile::Close(), ERROR: File does not exist" << std::endl;
            return 0;
        }

    }
    catch (exception e)
    {
        cerr << e.what();
        return 0;
    }
    return 1;
}

/*
    Adds a record to the database file.
    The new record is added at the end of the last available page
*/
void DBFile::Add(Record &rec)
{
    off_t fileSize = this->databaseFile.GetLength();

    int pageFull = this->databasePage.Append(&rec);
    if (pageFull == 0)  // which means current page is full
    {
        this->databasePage.EmptyItOut();
        this->databasePage.Append(&rec);
        this->databaseFile.AddPage(&this->databasePage, !fileSize ? 0 : fileSize - 1);
        pageNumber++;
    }
}

// Moves the file pointer to point the first record in the file
void DBFile::MoveFirst()
{
    if (this->databaseFile.GetLength() != 0)
    {
        this->databaseFile.GetPage(&this->databasePage, 0); //get first page
        cout << "Moved to first Page in Database File\n";
    }
    else
        cout << "Cannot MoveFirst in Database File as no pages are available \n";
}

/*
    Gets the next record in the file.
    Returns 1 if record is present; 0 if file is empty
*/
int DBFile::GetNext(Record &nextRecord)
{
    if (!this->databasePage.GetFirst(&nextRecord))
    {
        if (++pageNumber >= this->databaseFile.GetLength() - 1)
            return 0;

        this->databaseFile.GetPage(&this->databasePage, pageNumber);
        this->databasePage.GetFirst(&nextRecord);
    }
    return 1;
}

/*
    Gets the next record which matches the cnf expression.
    Returns 1 if record exists; 0 otherwise
*/
int DBFile::GetNext(Record &nextRecord, CNF &cnf, Record &literal)
{
    ComparisonEngine compEngine;
    while (this->GetNext(nextRecord))   // made changes corresponding to address of nextRec
    {
        if (compEngine.Compare(&nextRecord, &literal, &cnf))
            return 1;
    }
    return 0;
}

// Return length of file
int DBFile::GetLength()
{
    return this->databaseFile.GetLength();
}
