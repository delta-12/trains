/*****************************************************************************
* @file lookup_table_tests.cc
*
* @brief Unit testing for LookupTable.
*****************************************************************************/

#include <ranges>
#include <vector>

#include <gtest/gtest.h>

#include "lookup_table.h"
#include "types.h"

TEST(LookupTableTests, RowInsertTest)
{
    Row<types::BlockId, 75> row;

    ASSERT_FALSE(row.Contains(1));
    row.Insert(1);
    ASSERT_TRUE(row.Contains(1));
}

TEST(LookupTableTests, RowSetTest)
{
    Row<types::BlockId, 75> row;

    ASSERT_FALSE(row.Contains(1));
    row.Set(1, 0);
    ASSERT_TRUE(row.Contains(1));

    ASSERT_FALSE(row.Contains(2));
    row.Set(2, 1);
    ASSERT_TRUE(row.Contains(2));

    ASSERT_FALSE(row.Contains(3));
    row.Set(3, 0);
    ASSERT_TRUE(row.Contains(3));
    ASSERT_FALSE(row.Contains(1));

    row.Set(10, 75);
    ASSERT_FALSE(row.Contains(10));

    row.Set(10, -1);
    ASSERT_FALSE(row.Contains(10));
}

TEST(LookupTableTests, RowGetValuesTest)
{
    Row<types::BlockId, 75>     row;
    std::vector<types::BlockId> blocks;

    row.GetValues(blocks);
    ASSERT_EQ(0, blocks.size());

    row.Insert(1);
    row.Insert(2);
    row.Insert(3);
    row.GetValues(blocks);
    ASSERT_EQ(3, blocks.size());
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 1));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 2));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 3));

    row.Insert(3);
    row.GetValues(blocks);
    ASSERT_EQ(3, blocks.size());
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 1));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 2));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 3));

    row.Insert(4);
    row.GetValues(blocks);
    ASSERT_EQ(4, blocks.size());
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 1));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 2));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 3));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 4));
}

TEST(LookupTableTests, LookupTableTest)
{
    LookupTable<types::WaysideId, types::BlockId, 75> table;
    types::WaysideId                                  key;
    std::vector<types::BlockId>                       blocks;

    ASSERT_FALSE(table.GetKey(0, key));
    ASSERT_FALSE(table.GetValues(0, blocks));
    ASSERT_EQ(0, blocks.size());

    table.Insert(0, 1);
    table.Insert(0, 2);
    table.Insert(0, 3);
    table.Insert(0, 3);
    table.Insert(0, 8);
    table.Insert(1, 4);
    table.Insert(1, 5);
    table.Insert(1, 5);
    table.Insert(1, 6);

    ASSERT_TRUE(table.GetKey(1, key));
    ASSERT_EQ(0, key);
    ASSERT_TRUE(table.GetKey(2, key));
    ASSERT_EQ(0, key);
    ASSERT_TRUE(table.GetKey(3, key));
    ASSERT_EQ(0, key);
    ASSERT_TRUE(table.GetKey(4, key));
    ASSERT_EQ(1, key);
    ASSERT_TRUE(table.GetKey(5, key));
    ASSERT_EQ(1, key);
    ASSERT_TRUE(table.GetKey(6, key));
    ASSERT_EQ(1, key);
    ASSERT_FALSE(table.GetKey(7, key));

    ASSERT_FALSE(table.GetValues(2, blocks));
    ASSERT_TRUE(table.GetValues(0, blocks));
    ASSERT_EQ(4, blocks.size());
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 1));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 2));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 3));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 8));
    ASSERT_TRUE(table.GetValues(1, blocks));
    ASSERT_EQ(3, blocks.size());
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 4));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 5));
    ASSERT_NE(blocks.end(), std::ranges::find(blocks, 6));
}