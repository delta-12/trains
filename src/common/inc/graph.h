/*****************************************************************************
* @file graph.h
*
* @brief Implements a graph with builtin search methods.
*****************************************************************************/

#ifndef TRAINS_SRC_COMMON_INC_GRAPH_H
#define TRAINS_SRC_COMMON_INC_GRAPH_H

#include <unordered_map>

template<typename NodeLabel, typename EdgeWeight>
class Graph
{
    public:
        bool AddEdge(const NodeLabel &start, const NodeLabel &end, const EdgeWeight &weight);
        bool RemoveEdge(const NodeLabel &start, const NodeLabel &end);
        void Clear(void);
    // TODO adjust edge weight
    // TODO path algos

    private:
        std::unordered_map<NodeLabel, std::unordered_map<NodeLabel, EdgeWeight> > vertices_map_;
};

template<typename NodeLabel, typename EdgeWeight>
bool Graph<NodeLabel, EdgeWeight>::AddEdge(const NodeLabel &start, const NodeLabel &end, const EdgeWeight &weight)
{
    bool edge_added = true;

    // Check at least one vertex in connected to the graph
    if ((vertices_map_.end() == vertices_map_.find(start)) &&
        (vertices_map_.end() == vertices_map_.find(end)) &&
        (!vertices_map_.empty()))
    {
        edge_added = false;
    }
    // Check if edge is a duplicate
    else
    if (vertices_map_[start].end() != vertices_map_[start].find(end))
    {
        edge_added = false;
    }
    else
    {
        vertices_map_[start][end] = weight;
    }

    return edge_added;
}

template<typename NodeLabel, typename EdgeWeight>
bool Graph<NodeLabel, EdgeWeight>::RemoveEdge(const NodeLabel &start, const NodeLabel &end)
{
    bool edge_removed = true;

    // Check if edge exists
    if ((vertices_map_.end() == vertices_map_.find(start)) ||
        vertices_map_[start].end() == vertices_map_[start].find(end))
    {
        edge_removed = false;
    }
    else
    {
        vertices_map_[start].erase(end);

        if (vertices_map_[start].empty())
        {
            vertices_map_.erase(start);
        }
    }

    return edge_removed;
}

template<typename NodeLabel, typename EdgeWeight>
void Graph<NodeLabel, EdgeWeight>::Clear(void)
{
    vertices_map_.clear();
}

#endif // TRAINS_SRC_COMMON_INC_GRAPH_H