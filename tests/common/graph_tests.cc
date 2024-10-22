/*****************************************************************************
* @file graph_tests.cc
*
* @brief Unit testing for Graph.
*****************************************************************************/

#include <gtest/gtest.h>

#include "graph.h"
#include "types.h"

TEST(GraphTests, CopyConstructor)
{
    Graph<types::BlockId, types::Meters> graph;

    Graph<types::BlockId, types::Meters> graph_copy_0(graph);
    ASSERT_EQ(graph.GetEdgeCount(), graph_copy_0.GetEdgeCount());

    graph.AddEdge(0, 1, 1);
    graph.AddEdge(1, 2, 1);
    graph.AddEdge(1, 3, 1);
    graph.AddEdge(2, 3, 1);
    graph.AddEdge(3, 0, 1);
    ASSERT_NE(graph.GetEdgeCount(), graph_copy_0.GetEdgeCount());

    Graph<types::BlockId, types::Meters> graph_copy_1(graph);
    ASSERT_EQ(graph.GetEdgeCount(), graph_copy_1.GetEdgeCount());
    ASSERT_FALSE(graph_copy_1.AddEdge(0, 1, 1));
}

TEST(GraphTests, CopyAssignment)
{
    Graph<types::BlockId, types::Meters> graph;

    Graph<types::BlockId, types::Meters> graph_copy_0;
    graph_copy_0 = graph;
    ASSERT_EQ(graph.GetEdgeCount(), graph_copy_0.GetEdgeCount());

    graph.AddEdge(0, 1, 1);
    graph.AddEdge(1, 2, 1);
    graph.AddEdge(1, 3, 1);
    graph.AddEdge(2, 3, 1);
    graph.AddEdge(3, 0, 1);
    ASSERT_NE(graph.GetEdgeCount(), graph_copy_0.GetEdgeCount());

    Graph<types::BlockId, types::Meters> graph_copy_1;
    graph_copy_1 = graph;
    ASSERT_EQ(graph.GetEdgeCount(), graph_copy_1.GetEdgeCount());
    ASSERT_FALSE(graph_copy_1.AddEdge(0, 1, 1));

    Graph<types::BlockId, types::Meters> graph_copy_2 = graph;
    ASSERT_EQ(graph.GetEdgeCount(), graph_copy_2.GetEdgeCount());
    ASSERT_FALSE(graph_copy_1.AddEdge(0, 1, 1));

    Graph<types::BlockId, types::Meters> graph_copy_3;
    ASSERT_TRUE(graph_copy_3.AddEdge(5, 6, 1));
    graph_copy_3 = graph;
    ASSERT_EQ(graph.GetEdgeCount(), graph_copy_3.GetEdgeCount());
    ASSERT_FALSE(graph_copy_3.AddEdge(5, 6, 1));
    ASSERT_FALSE(graph_copy_3.AddEdge(5, 7, 1));
    ASSERT_TRUE(graph_copy_3.AddEdge(7, 0, 1));
    ASSERT_TRUE(graph_copy_3.AddEdge(5, 0, 1));
    ASSERT_TRUE(graph_copy_3.AddEdge(5, 6, 1));
    ASSERT_TRUE(graph_copy_3.AddEdge(5, 7, 1));
}

TEST(GraphTests, AddEdges)
{
    Graph<types::BlockId, types::Meters> graph;

    ASSERT_EQ(0, graph.GetEdgeCount());

    ASSERT_TRUE(graph.AddEdge(0, 0, 5));
    ASSERT_EQ(1, graph.GetEdgeCount());

    ASSERT_FALSE(graph.AddEdge(1, 1, 6));
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

    // Does not support multigraphs
    ASSERT_FALSE(graph.AddEdge(0, 3, 1));
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

TEST(GraphTests, RemoveEdges)
{
    Graph<types::BlockId, types::Meters> graph;

    ASSERT_FALSE(graph.RemoveEdge(0, 1));

    graph.AddEdge(0, 0, 1);
    graph.AddEdge(0, 1, 1);
    graph.AddEdge(1, 1, 1);
    graph.AddEdge(0, 2, 1);
    graph.AddEdge(0, 3, 1);
    graph.AddEdge(0, 4, 1);
    graph.AddEdge(0, 5, 1);
    ASSERT_EQ(7, graph.GetEdgeCount());
    ASSERT_TRUE(graph.RemoveEdge(0, 3));
    ASSERT_FALSE(graph.RemoveEdge(1, 0));
    ASSERT_TRUE(graph.RemoveEdge(0, 1));
    ASSERT_EQ(5, graph.GetEdgeCount());
    ASSERT_TRUE(graph.RemoveEdge(0, 2));
    ASSERT_FALSE(graph.RemoveEdge(0, 2));
    ASSERT_TRUE(graph.RemoveEdge(1, 1));
    ASSERT_TRUE(graph.RemoveEdge(0, 4));
    ASSERT_TRUE(graph.RemoveEdge(0, 5));
    ASSERT_TRUE(graph.RemoveEdge(0, 0));
    ASSERT_EQ(0, graph.GetEdgeCount());
    ASSERT_FALSE(graph.RemoveEdge(0, 0));

    graph.AddEdge(0, 2, 1);
    graph.AddEdge(1, 2, 1);
    graph.AddEdge(2, 3, 1);
    graph.AddEdge(3, 4, 1);
    ASSERT_EQ(4, graph.GetEdgeCount());
    ASSERT_FALSE(graph.RemoveEdge(2, 1));
    ASSERT_TRUE(graph.RemoveEdge(2, 3));
    ASSERT_TRUE(graph.RemoveEdge(1, 2));
    ASSERT_EQ(2, graph.GetEdgeCount());

    // Adding and removing edges from disconnected graphs
    ASSERT_TRUE(graph.AddEdge(4, 5, 1));
    ASSERT_TRUE(graph.AddEdge(6, 3, 1));
    ASSERT_TRUE(graph.AddEdge(2, 7, 1));
    ASSERT_EQ(5, graph.GetEdgeCount());

    ASSERT_TRUE(graph.RemoveEdge(0, 2));
    ASSERT_TRUE(graph.RemoveEdge(3, 4));
    ASSERT_TRUE(graph.AddEdge(5, 0, 1));
    ASSERT_EQ(4, graph.GetEdgeCount());

    ASSERT_TRUE(graph.RemoveEdge(4, 5));
    ASSERT_TRUE(graph.RemoveEdge(2, 7));
    ASSERT_TRUE(graph.RemoveEdge(6, 3));
    ASSERT_TRUE(graph.RemoveEdge(5, 0));
    ASSERT_EQ(0, graph.GetEdgeCount());
}

TEST(GraphTests, BreadthFirstSearchConnectedNodes)
{
    Graph<types::BlockId, types::Meters> graph;
    std::unordered_set<types::BlockId>   connected_blocks;

    ASSERT_EQ(0, graph.BreadthFirstSearch(0).size());

    graph.AddEdge(1, 2, 1);
    graph.AddEdge(1, 3, 1);
    graph.AddEdge(2, 3, 1);
    graph.AddEdge(3, 4, 1);
    graph.AddEdge(4, 5, 1);
    graph.AddEdge(4, 6, 1);
    ASSERT_EQ(0, graph.BreadthFirstSearch(7).size());
    connected_blocks = graph.BreadthFirstSearch(1);
    ASSERT_EQ(6, connected_blocks.size());
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(1));
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(2));
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(3));
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(4));
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(5));
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(6));
    ASSERT_EQ(3, graph.BreadthFirstSearch(4).size());

    graph.RemoveEdge(3, 4);
    connected_blocks = graph.BreadthFirstSearch(1);
    ASSERT_EQ(3, connected_blocks.size());
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(1));
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(2));
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(3));
    ASSERT_EQ(connected_blocks.end(), connected_blocks.find(4));
    connected_blocks = graph.BreadthFirstSearch(4);
    ASSERT_EQ(3, connected_blocks.size());
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(4));
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(5));
    ASSERT_NE(connected_blocks.end(), connected_blocks.find(6));
}

TEST(GraphTests, Dijkstra)
{
    Graph<types::BlockId, types::Meters> graph;
    std::vector<types::BlockId>          path;

    graph.AddEdge(1, 2, 1);
    graph.AddEdge(1, 3, 1);
    graph.AddEdge(2, 3, 1);
    graph.AddEdge(3, 4, 1);
    graph.AddEdge(4, 5, 1);
    graph.AddEdge(4, 6, 1);
    graph.AddEdge(5, 6, 1);
    ASSERT_EQ(7, graph.GetEdgeCount());

    path = graph.Dijkstra(1, 6);
    ASSERT_EQ(4, path.size());
    ASSERT_EQ(1, path[0]);
    ASSERT_EQ(3, path[1]);
    ASSERT_EQ(4, path[2]);
    ASSERT_EQ(6, path[3]);

    path = graph.Dijkstra(2, 6);
    ASSERT_EQ(4, path.size());
    ASSERT_EQ(2, path[0]);
    ASSERT_EQ(3, path[1]);
    ASSERT_EQ(4, path[2]);
    ASSERT_EQ(6, path[3]);

    path = graph.Dijkstra(3, 6);
    ASSERT_EQ(3, path.size());
    ASSERT_EQ(3, path[0]);
    ASSERT_EQ(4, path[1]);
    ASSERT_EQ(6, path[2]);

    path = graph.Dijkstra(4, 6);
    ASSERT_EQ(2, path.size());
    ASSERT_EQ(4, path[0]);
    ASSERT_EQ(6, path[1]);

    path = graph.Dijkstra(0, 6);
    ASSERT_EQ(0, path.size());

    path = graph.Dijkstra(4, 1);
    ASSERT_EQ(0, path.size());

    path = graph.Dijkstra(6, 0);
    ASSERT_EQ(0, path.size());

    path = graph.Dijkstra(1, 1);
    ASSERT_EQ(1, path.size());
    ASSERT_EQ(1, path[0]);

    graph.RemoveEdge(1, 3);
    graph.AddEdge(1, 3, 3);
    path = graph.Dijkstra(1, 6);
    ASSERT_EQ(5, path.size());
    ASSERT_EQ(1, path[0]);
    ASSERT_EQ(2, path[1]);
    ASSERT_EQ(3, path[2]);
    ASSERT_EQ(4, path[3]);
    ASSERT_EQ(6, path[4]);

    graph.RemoveEdge(4, 6);
    graph.AddEdge(4, 6, 3);
    path = graph.Dijkstra(1, 6);
    ASSERT_EQ(6, path.size());
    ASSERT_EQ(1, path[0]);
    ASSERT_EQ(2, path[1]);
    ASSERT_EQ(3, path[2]);
    ASSERT_EQ(4, path[3]);
    ASSERT_EQ(5, path[4]);
    ASSERT_EQ(6, path[5]);

    graph.Clear();
    graph.AddEdge(1, 2, 1);
    graph.AddEdge(1, 3, 3);
    graph.AddEdge(2, 1, 1);
    graph.AddEdge(2, 3, 1);
    graph.AddEdge(3, 1, 2);
    graph.AddEdge(3, 2, 1);
    graph.AddEdge(3, 4, 1);
    graph.AddEdge(4, 3, 1);
    graph.AddEdge(4, 5, 1);
    graph.AddEdge(4, 7, 5);
    graph.AddEdge(5, 3, 6);
    graph.AddEdge(5, 6, 1);
    graph.AddEdge(6, 7, 1);
    graph.AddEdge(7, 4, 1);
    ASSERT_EQ(14, graph.GetEdgeCount());

    path = graph.Dijkstra(1, 7);
    ASSERT_EQ(7, path.size());
    ASSERT_EQ(1, path[0]);
    ASSERT_EQ(2, path[1]);
    ASSERT_EQ(3, path[2]);
    ASSERT_EQ(4, path[3]);
    ASSERT_EQ(5, path[4]);
    ASSERT_EQ(6, path[5]);
    ASSERT_EQ(7, path[6]);

    path = graph.Dijkstra(7, 1);
    ASSERT_EQ(4, path.size());
    ASSERT_EQ(7, path[0]);
    ASSERT_EQ(4, path[1]);
    ASSERT_EQ(3, path[2]);
    ASSERT_EQ(1, path[3]);

    path = graph.Dijkstra(5, 2);
    ASSERT_EQ(6, path.size());
    ASSERT_EQ(5, path[0]);
    ASSERT_EQ(6, path[1]);
    ASSERT_EQ(7, path[2]);
    ASSERT_EQ(4, path[3]);
    ASSERT_EQ(3, path[4]);
    ASSERT_EQ(2, path[5]);
}