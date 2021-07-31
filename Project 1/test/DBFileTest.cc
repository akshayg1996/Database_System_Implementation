#include "../DBFile.h"
#include <gtest/gtest.h>
#include <iostream>

TEST(DBFileTest, testCreateSuccess)
{
    const char *tpch_dir = "test/data/lineitem.tbl";
    DBFile h;
    int createHeap = h.Create(tpch_dir, heap, NULL);
    ASSERT_EQ(createHeap, 1);
}

TEST(DBFileTest, testCreateFailure)
{
    const char *tpch_dir = "test/data/lineitem.tbl";
    DBFile h;
    int createHeap = h.Create(tpch_dir, tree, NULL);
    ASSERT_EQ(createHeap, 1);
}


TEST(DBFileTest, testOpenSuccess)
{
    const char *tpch_dir = "test/data/lineitem.tbl";
    DBFile h;
    int createHeap = h.Create(tpch_dir, heap, NULL);
    ASSERT_EQ(createHeap, 1);
    int openFile = h.Open(tpch_dir);
    ASSERT_EQ(openFile, 1);
}

TEST(DBFileTest, testOpenFailure)
{
    const char *tpch_dir = "test/data/sample.tbl";
    DBFile h;
    int openFile = h.Open(tpch_dir);
    ASSERT_EQ(openFile, 0);
}

TEST(DBFileTest, testCloseSuccess)
{
    const char *tpch_dir = "test/data/lineitem.tbl";
    DBFile h;
    int openFile = h.Open(tpch_dir);
    ASSERT_EQ(openFile, 1);
    int closeFile = h.Close();
    ASSERT_EQ(closeFile, 1);
}

TEST(DBFileTest, testCloseFailure)
{
    DBFile h;
    int closeFile = h.Close();
    ASSERT_EQ(closeFile, 0);
}