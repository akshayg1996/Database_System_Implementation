#include "gtest/gtest.h"
#include "../ParseTree.h"
#include "../QueryPlan.h"
#define EXPECTED 1

char* catalog_path = "catalog";
char* dbfile_dir = "bin/";
char* tpch_dir = "tpch-dbgen/";

TEST(QUERY_PLAN, CREATE_LEAF_NODE)
{
Statistics *s;
QueryPlan query(s);
int actual = query.PRojectNodeCreation();
ASSERT_EQ(EXPECTED, actual);
}

TEST(QUERY_PLAN, FIND_LEAST_COST_JOIN)
{
Statistics *s;
QueryPlan query(s);
int actual = query.SumNodes();
ASSERT_EQ(EXPECTED, actual);
}