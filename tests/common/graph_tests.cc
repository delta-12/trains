/*****************************************************************************
* @file graph_tests.cc
*
* @brief Unit testing for Graph.
*****************************************************************************/

#include <gtest/gtest.h>

#include "graph.h"
#include "types.h"

TEST(GraphTests, AddEdge)
{
    Graph<types::BlockId, types::Meters> graph;

    graph.AddEdge(0, 1, 5);
}