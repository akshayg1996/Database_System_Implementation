#include "../DBFile.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../MetaDataHandler.h"

TEST(DBFileTest, testCreateHeapFile) {
    DBFile dbfile;

    int result = dbfile.Create("sample.bin", heap, NULL);

    ASSERT_EQ(1, result);
}

TEST(DBFileTest, testCreateSortedFile) {
    DBFile dbfile;
    OrderMaker o;
    struct {OrderMaker *o; int l;} startup = {&o, 1};

    int result = dbfile.Create("sample.bin", sorted, &startup);

    ASSERT_EQ(1, result);
}

TEST(DBFileTest, testLoadFile) {
    Schema f_schema("catalog", "nation");
    DBFile dbfile;

    dbfile.Load(f_schema, "sample.bin");

}

TEST(DBFileTest, testOpenHeapFile) {
    DBFile dbfile;

    int result = dbfile.Open("test/data/nation.bin");

    ASSERT_EQ(1, result);
}

TEST(DBFileTest, testOpenSortedFile) {
    DBFile dbfile;

    int result = dbfile.Open("test/data/region.bin");

    ASSERT_EQ(1, result);
}

TEST(DBFileTest, testClose) {
    DBFile dbfile;

    dbfile.Open("test/data/nation.bin");
    int result = dbfile.Close();

    ASSERT_EQ(1, result);
}

