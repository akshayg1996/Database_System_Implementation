#include "BigQ.h"

BigQ::BigQ(Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen, const char *fpath) : inputPipe(in), outputPipe(out), orderMaker(sortorder)
{
    this->runLength = runlen;
    this->SetNoOfRecordsPerPage(fpath);
    this->OpenFile("tempFile.bin");
    int rs = pthread_create(&threadID, NULL, BigQ::SortAndDumpToPipe, (void *)this);
    if (rs)
    {
        printf("Error creating worker thread! Return %d\n", rs);
        exit(-1);
    }
}

/*
    Opens database file.
    Returns 1 on success; 0 on failure
*/
int BigQ::OpenFile(string fileName)
{
    try
    {
        this->tempFile = fileName;
        auto f_path = const_cast<char *>(this->tempFile.c_str());
        if(!std::ifstream(strdup(f_path)))
        {
            cerr << "ERROR: File does not exist or could not be opened" << std::endl;
            return 0;
        }
        this->file.Open(0, f_path);
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
int BigQ::CloseFile()
{
    try
    {
        auto f_path = const_cast<char *>(this->tempFile.c_str());
        ifstream file(f_path);
        if(file.is_open())
            this->file.Close();
        else
        {
            cerr << "In BigQ::Close(), ERROR: File does not exist" << std::endl;
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

BigQ::~BigQ()
{
}

void *BigQ::SortAndDumpToPipe(void *Big)
{
    BigQ *bigQ = (BigQ *) Big;
    OrderMaker o1 = bigQ->orderMaker;
    OrderMaker o2 = bigQ->orderMaker;

    auto comp = [&](Record *p, Record *q)
    {
        ComparisonEngine comp;
        return comp.Compare(p, q, &o1) < 0;
    };

    auto comp1 = [&](pair<int, Record *> p, pair<int, Record *> q)
    {
        ComparisonEngine comp;
        return comp.Compare(p.second, q.second, &o2) < 0;
    };

    Record temp;
    Page *writePage = new Page();
    size_t curLength = 0;
    int numPages = 0;
    vector<Record *> result; //this will be used to sort the values internally within reach run;
    vector<int> pageCounter;
    int count = 0;
    int recordCount = 0;
    while ((bigQ->inputPipe).Remove(&temp))
    {
        if (recordCount <= (bigQ->noOfRecordsPerPage * bigQ->runLength))
        {
            //cout << "Processing " << ++recordCount << " record now" << endl;
            Record *rec = new Record();
            rec->Consume(&temp);
            result.push_back(rec);
        }
        else
        {
            //cout << "Processed " << recordCount << " records." << endl;
            //cout << "Going to sort now." << endl;
            std::sort(result.begin(), result.end(), comp);
            pageCounter.push_back(numPages);
            for (auto i:result)
            {
                int res = writePage->Append(i);
                if (!res)
                {
                    int pos = bigQ->file.GetLength();
                    pos = (pos == 0 ? 0 : (pos - 1));
                    bigQ->file.AddPage(writePage, pos);
                    writePage->EmptyItOut();
                    writePage->Append(i);
                    numPages++;
                }
            }
            int position = bigQ->file.GetLength();
            bigQ->file.AddPage(writePage, (!position) ? 0 : (position - 1));
            numPages++;

            for (auto i:result)
                delete i;
            result.clear();
            Record *rec = new Record();
            rec->Consume(&temp);
            result.push_back(rec);
            recordCount++;
        }
    }
    if (!result.empty())
    {
        // If its not the last record in a page, have to append records to a new page.
        std::sort(result.begin(), result.end(), comp);
        pageCounter.push_back(numPages);
        for (auto i:result)
        {
            int res = writePage->Append(i);
            if (!res)
            {
                int pos = bigQ->file.GetLength();
                pos = (pos == 0 ? 0 : (pos - 1));
                bigQ->file.AddPage(writePage, pos);
                writePage->EmptyItOut();
                writePage->Append(i);
                numPages++;
            }
        }
        int position = bigQ->file.GetLength();
        bigQ->file.AddPage(writePage, (!position) ? 0 : (position - 1));
        numPages++;

        for (auto i:result)
            delete i;
        result.clear();
    }

    // Load to Priority Queue and dump to output pipe
    pageCounter.push_back(numPages);
    vector<Page *> pageKeeper;
    priority_queue<pair<int, Record *>, vector<pair<int, Record *>>, decltype(comp1)> pq(comp1);
    for (int i = 0; i < pageCounter.size() - 1; i++)
    {
        Page *temp_P = new Page();
        bigQ->file.GetPage(temp_P, pageCounter[i]);
        Record *temp_R = new Record();
        temp_P->GetFirst(temp_R);
        //cout << "Before pushing into Priority Queue, i = " << i << endl;
        pq.push(make_pair(i, temp_R));
        pageKeeper.push_back(temp_P);
    }
    //cout << "Size of Priority Queue = " << pq.size() << endl;
    //cout << "Size of pageKeeper = " << pageKeeper.size() << endl;

    //cout << "PageCounter = ";
//    for (std::vector<int>::const_iterator i = pageCounter.begin(); i != pageCounter.end(); ++i)
//        std::cout << *i << ' ';

    //cout << endl;

    vector<int> pageChecker(pageCounter);
    //cout << "Dumping records to output pipe" << endl;
    while (!pq.empty())
    {
        auto top = pq.top();
        //cout << "Before pop(), top.First = " << top.first << endl;
        //cout << "Before pop(), top.second = " << top.second << endl;
        bigQ->outputPipe.Insert(top.second);
        pq.pop();
        //cout << "After pop(), top.First = " << top.first << endl;
        //cout << "After pop(), top.second = " << top.second << endl;
        Record *temp_R = new Record();
        //cout << "PageKeeper[top.first] = " << pageKeeper[top.first] << endl;
        if (!pageKeeper[top.first]->GetFirst(temp_R))
        {
            //cout << "Hit 1" << endl;
            if (++pageChecker[top.first] < pageCounter[top.first + 1])
            {
                //cout << "Hit 2" << endl;
                pageKeeper[top.first]->EmptyItOut();
                bigQ->file.GetPage(pageKeeper[top.first], pageChecker[top.first]);
                pageKeeper[top.first]->GetFirst(temp_R);
                pq.push(make_pair(top.first, temp_R));
            }
        }
        else
        {
            //cout << "In else:" << " top.first = " << top.first << endl;
            //cout << "In else:" << " top.second = " << top.second << endl;
            pq.push(make_pair(top.first, temp_R));
        }
    }
    for (auto i:pageKeeper)
        delete i;
    bigQ->outputPipe.ShutDown();
    bigQ->CloseFile();
    //bigQ->file.Close();
    //remove((bigQ->tempFile).c_str());
    //remove((bigQ->tempFile + ".meta").c_str());
    return NULL;
}