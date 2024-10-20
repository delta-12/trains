/*****************************************************************************
* @file graph.h
*
* @brief Implements a directed graph with builtin search methods.
*****************************************************************************/

#ifndef TRAINS_SRC_COMMON_INC_GRAPH_H
#define TRAINS_SRC_COMMON_INC_GRAPH_H

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>

template <typename NodeLabel, typename EdgeWeight>
class Graph
{
    public:
        bool AddEdge(const NodeLabel &start, const NodeLabel &end, const EdgeWeight &weight);
        bool RemoveEdge(const NodeLabel &start, const NodeLabel &end);
        void Clear(void);
        size_t GetEdgeCount(void) const;
        std::unordered_set<NodeLabel> BreadthFirstSearch(const NodeLabel &start) const;
        bool BreadthFirstSearch(const NodeLabel &start, const NodeLabel &end, std::vector<NodeLabel> &path) const;
    // TODO NNF-190 path algorithms

    private:
        bool IsConnected(const NodeLabel &node) const;
        std::unordered_map<NodeLabel, std::unordered_map<NodeLabel, EdgeWeight>> node_map_;
};

template <typename NodeLabel, typename EdgeWeight>
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
template <typename NodeLabel, typename EdgeWeight>
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

template <typename NodeLabel, typename EdgeWeight>
void Graph<NodeLabel, EdgeWeight>::Clear(void)
{
    node_map_.clear();
}

template <typename NodeLabel, typename EdgeWeight>
size_t Graph<NodeLabel, EdgeWeight>::GetEdgeCount(void) const
{
    size_t edge_count = 0;

    for (const typename std::unordered_map<NodeLabel, std::unordered_map<NodeLabel, EdgeWeight>>::value_type &edges : node_map_)
    {
        edge_count += edges.second.size();
    }

    return edge_count;
}

template <typename NodeLabel, typename EdgeWeight>
bool Graph<NodeLabel, EdgeWeight>::IsConnected(const NodeLabel &node) const
{
    bool connected = false;

    if (node_map_.end() != node_map_.find(node))
    {
        connected = true;
    }
    else if (node_map_.end() != std::find_if(node_map_.begin(), node_map_.end(), [node](const std::unordered_map<NodeLabel, std::unordered_map<NodeLabel, EdgeWeight>>::value_type &edges)
    {
        return edges.second.end() != edges.second.find(node);
    }))
    {
        connected = true;
    }

    return connected;
}

template <typename NodeLabel, typename EdgeWeight>
std::unordered_set<NodeLabel> Graph<NodeLabel, EdgeWeight>::BreadthFirstSearch(const NodeLabel &start) const
{
    std::queue<NodeLabel>         node_queue;
    std::unordered_set<NodeLabel> visited_nodes;

    if (IsConnected(start))
    {
        node_queue.push(start);
        visited_nodes.insert(start);

        while (!node_queue.empty())
        {
            typename std::unordered_map<NodeLabel, std::unordered_map<NodeLabel, EdgeWeight>>::const_iterator current_node = node_map_.find(node_queue.front());
            node_queue.pop();

            if (node_map_.end() != current_node)
            {
                for (const typename std::unordered_map<NodeLabel, EdgeWeight>::value_type &edge : current_node->second)
                {
                    if (visited_nodes.end() == visited_nodes.find(edge.first))
                    {
                        visited_nodes.insert(edge.first);
                        node_queue.push(edge.first);
                    }
                }
            }
        }
    }

    return visited_nodes;
}

template <typename NodeLabel, typename EdgeWeight>
bool Graph<NodeLabel, EdgeWeight>::BreadthFirstSearch(const NodeLabel &start, const NodeLabel &end, std::vector<NodeLabel> &path) const
{
    bool connected = false;
    // std::queue<NodeLabel>         node_queue;
    // std::unordered_set<NodeLabel> visited_nodes;
    // std::unordered_map<NodeLabel, NodeLabel> parent_nodes;

    // node_queue.push(start);
    // visited_nodes.insert(start);

    // while (!node_queue.empty())
    // {
    //     NodeLabel current_node = node_queue.front();
    //     node_queue.pop();

    //     for (const std::unordered_map<NodeLabel, EdgeWeight>::value_type &edge : node_map_[current_node])
    //     {
    //         if (visited_nodes.end() == visited_nodes.find(edge.first))
    //         {
    //             visited_nodes.insert(edge.first);
    //             node_queue.push(edge.first);
    //         }
    //     }
    // }

    return connected;
}

#endif // TRAINS_SRC_COMMON_INC_GRAPH_H