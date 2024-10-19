/*****************************************************************************
* @file graph_tests.cc
*
* @brief Unit testing for Graph.
*****************************************************************************/

#include <gtest/gtest.h>

#include "graph.h"
#include "types.h"

TEST(GraphTests, AddEdges)
{
    Graph<types::BlockId, types::Meters> graph;

    ASSERT_EQ(0, graph.GetEdgeCount());

    ASSERT_TRUE(graph.AddEdge(0, 0, 5));
    ASSERT_EQ(1, graph.GetEdgeCount());

    ASSERT_FALSE(graph.AddEdge(1, 2, 6));
    ASSERT_EQ(1, graph.GetEdgeCount());

    ASSERT_TRUE(graph.AddEdge(0, 1, 6));
    ASSERT_EQ(2, graph.GetEdgeCount());

    ASSERT_FALSE(graph.AddEdge(0, 1, 6));
    ASSERT_FALSE(graph.AddEdge(2, 3, 1));
    ASSERT_EQ(2, graph.GetEdgeCount());

    ASSERT_TRUE(graph.AddEdge(2, 1, 3));
    ASSERT_EQ(3, graph.GetEdgeCount());

    ASSERT_TRUE(graph.AddEdge(1, 2, 10));
    ASSERT_EQ(4, graph.GetEdgeCount());

    ASSERT_TRUE(graph.AddEdge(0, 3, 7));
    ASSERT_EQ(5, graph.GetEdgeCount());
}

TEST(GraphTests, Clear)
{
    Graph<types::BlockId, types::Meters> graph;

    ASSERT_EQ(0, graph.GetEdgeCount());
    graph.Clear();
    ASSERT_EQ(0, graph.GetEdgeCount());

    graph.AddEdge(0, 1, 1);
    graph.AddEdge(0, 2, 1);
    graph.AddEdge(0, 3, 1);
    graph.AddEdge(0, 4, 1);
    graph.AddEdge(0, 5, 1);
    ASSERT_EQ(5, graph.GetEdgeCount());
    graph.Clear();
    ASSERT_EQ(0, graph.GetEdgeCount());

    graph.AddEdge(6, 7, 1);
    ASSERT_EQ(1, graph.GetEdgeCount());
    graph.Clear();
    ASSERT_EQ(0, graph.GetEdgeCount());
}