/*****************************************************************************
* @file graph.h
*
* @brief Implements a graph with builtin search methods.
*****************************************************************************/

#ifndef TRAINS_SRC_COMMON_INC_GRAPH_H
#define TRAINS_SRC_COMMON_INC_GRAPH_H

#include <algorithm>
#include <unordered_map>

template<typename NodeLabel, typename EdgeWeight>
class Graph
{
    public:
        bool AddEdge(const NodeLabel &start, const NodeLabel &end, const EdgeWeight &weight);
        bool RemoveEdge(const NodeLabel &start, const NodeLabel &end);
        void Clear(void);
        size_t GetEdgeCount(void) const;
    // TODO NNF-190 path algorithms

    private:
        bool IsConnected(const NodeLabel &node) const;
        std::unordered_map<NodeLabel, std::unordered_map<NodeLabel, EdgeWeight>> node_map_;
};

template<typename NodeLabel, typename EdgeWeight>
bool Graph<NodeLabel, EdgeWeight>::AddEdge(const NodeLabel &start, const NodeLabel &end, const EdgeWeight &weight)
{
    bool edge_added = true;

    // Check at least one node is connected to the graph
    if ((!IsConnected(start)) && (!IsConnected(end)) && (!node_map_.empty()))
    {
        edge_added = false;
    }
    // Check if the edge is a duplicate
    else if (node_map_[start].end() != node_map_[start].find(end))
    {
        edge_added = false;
    }
    else
    {
        node_map_[start][end] = weight;
    }

    return edge_added;
}

// Allows for disconnected graphs to form
template<typename NodeLabel, typename EdgeWeight>
bool Graph<NodeLabel, EdgeWeight>::RemoveEdge(const NodeLabel &start, const NodeLabel &end)
{
    bool edge_removed = true;

    // Check if edge exists
    if (node_map_.end() == node_map_.find(start))
    {
        edge_removed = false;
    }
    else if (node_map_[start].end() == node_map_[start].find(end))
    {
        edge_removed = false;
    }
    else
    {
        node_map_[start].erase(end);


        if (node_map_[start].empty())
        {
            node_map_.erase(start);
        }
    }

    return edge_removed;
}

template<typename NodeLabel, typename EdgeWeight>
void Graph<NodeLabel, EdgeWeight>::Clear(void)
{
    node_map_.clear();
}

template<typename NodeLabel, typename EdgeWeight>
size_t Graph<NodeLabel, EdgeWeight>::GetEdgeCount(void) const
{
    size_t edge_count = 0;

    for (const typename std::unordered_map<NodeLabel, std::unordered_map<NodeLabel, EdgeWeight>>::value_type &edges : node_map_)
    {
        edge_count += edges.second.size();
    }

    return edge_count;
}

template<typename NodeLabel, typename EdgeWeight>
bool Graph<NodeLabel, EdgeWeight>::IsConnected(const NodeLabel &node) const
{
    bool connected = false;

    if (node_map_.end() != node_map_.find(node))
    {
        connected = true;
    }
    else if (node_map_.end() != std::find_if(node_map_.begin(), node_map_.end(), [node](const std::unordered_map<NodeLabel, std::unordered_map<NodeLabel, EdgeWeight>>::value_type &edges){
        return edges.second.end() != edges.second.find(node);
    }))
    {
        connected = true;
    }

    return connected;
}

#endif // TRAINS_SRC_COMMON_INC_GRAPH_H