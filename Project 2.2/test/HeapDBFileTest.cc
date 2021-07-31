#include "../DBFile.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"



TEST(HeapDBFileTest, testGetNext) {
    DBFile dbfile;
    Record temp;
    dbfile.Open ("test/data/nation.bin");
    dbfile.MoveFirst ();
    int result = 0;

    if (dbfile.GetNext (temp)) {
        result= 1;
    }
    dbfile.Close ();

    ASSERT_EQ(1, result);
}