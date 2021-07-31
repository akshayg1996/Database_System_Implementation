#include "../DBFile.h"
#include "../BigQ.h"
#include <gtest/gtest.h>
#include <iostream>
#include <bits/types/FILE.h>
#include <pthread.h>
#include <vector>
#include "../Comparison.h"
#include "../ComparisonEngine.h"
#include "../File.h"
#include "../Schema.h"

// Test to validate successful opening of file
TEST(BigQTest, testOpenSuccess)
{
    OrderMaker sortorder;
    int buffsz = 100; // pipe cache size
    Pipe input(buffsz);
    Pipe output(buffsz);

    BigQ bq(input, output, sortorder, 2, "test/data/nation.bin");

    ASSERT_EQ(1, bq.OpenFile("tempfile.bin"));
}

// Test to validate error on opening of file
TEST(BigQTest, testOpenFailure)
{
    OrderMaker sortorder;
    int buffsz = 100; // pipe cache size
    Pipe input(buffsz);
    Pipe output(buffsz);

    BigQ bq(input, output, sortorder, 2, "test/data/nation.bin");

    ASSERT_EQ(0, bq.OpenFile("test.bin"));
}

// Test to validate successful closing of file
TEST(BigQTest, testCloseSuccess)
{
    OrderMaker sortorder;
    int buffsz = 100; // pipe cache size
    Pipe input(buffsz);
    Pipe output(buffsz);

    BigQ bq(input, output, sortorder, 2, "test/data/nation.bin");
    bq.OpenFile("tempfile.bin");
    ASSERT_EQ(1, bq.CloseFile());
}

// Test to validate error on closing of file
TEST(BigQTest, testCloseFailure)
{
    OrderMaker sortorder;
    int buffsz = 100; // pipe cache size
    Pipe input(buffsz);
    Pipe output(buffsz);

    BigQ bq(input, output, sortorder, 2, "test/data/nation.bin");
    bq.OpenFile("test.bin");
    ASSERT_EQ(0, bq.CloseFile());
}

//Test to determine if data is being inserted into the pipe
TEST(BigQTest, testPipeInsertion)
{
    Pipe input(100);
    int containsRecords, recordCounter = 0;;
    Record tempRecord, getRecord;
    DBFile dbFile;
    dbFile.Open("test/data/nation.bin");
    dbFile.MoveFirst();
    while (dbFile.GetNext(tempRecord) == 1)
    {
        recordCounter += 1;
        input.Insert(&tempRecord);
    }
    containsRecords = input.Remove(&getRecord);
    dbFile.Close();
    input.ShutDown();
    EXPECT_TRUE(containsRecords);
}

//Test to validate functioning of SortAndDump thread routine on NULL input
TEST(BigQTest, testSortAndDump)
{
    Pipe input(100);
    Pipe output(100);
    OrderMaker sortorder;

    BigQ bq(input, output, sortorder, 2, "test/data/nation.bin");
    ASSERT_EXIT(BigQ::SortAndDumpToPipe(NULL),::testing::KilledBySignal(SIGSEGV),".*");
}

//Test to demonstrate if bin file is empty or not
TEST(BigQTest, testBinFileNotEmpty) {
    int isNotEmpty;
    Record tempRecord;
    DBFile dbfile;
    dbfile.Open ("test/data/nation.bin");
    dbfile.MoveFirst ();
    isNotEmpty = dbfile.GetNext (tempRecord);
    EXPECT_TRUE(isNotEmpty);
}
