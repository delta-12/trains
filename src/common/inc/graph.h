/*****************************************************************************
* @file graph.h
*
* @brief Implements a directed graph with builtin search methods.
*****************************************************************************/

#ifndef TRAINS_SRC_COMMON_INC_GRAPH_H
#define TRAINS_SRC_COMMON_INC_GRAPH_H

#include <algorithm>
#include <functional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Does not support multigraphs
template <typename NodeLabel, typename EdgeWeight>
class Graph
{
    public:
        Graph(void);
        Graph(const Graph<NodeLabel, EdgeWeight> &graph);
        Graph& operator=(const Graph<NodeLabel, EdgeWeight>& graph);
        bool AddEdge(const NodeLabel start, const NodeLabel end, const EdgeWeight &weight);
        bool RemoveEdge(const NodeLabel start, const NodeLabel end);
        void Clear(void);
        size_t GetEdgeCount(void) const;
        std::unordered_set<NodeLabel> BreadthFirstSearch(const NodeLabel start) const;
        std::vector<NodeLabel> Dijkstra(const NodeLabel start, const NodeLabel end) const;

    private:
        typedef struct NodeDistance
        {
            NodeDistance(void)
            {
            }
            NodeDistance(const NodeLabel node, const EdgeWeight distance) : node(node), distance(distance)
            {
            }
            NodeDistance(const NodeLabel node, const NodeLabel previous, const EdgeWeight distance) : node(node), previous(previous), distance(distance)
            {
            }
            NodeLabel node;
            NodeLabel previous;
            EdgeWeight distance;
        } NodeDistance;

        bool IsConnected(const NodeLabel node) const;
        std::unordered_map<NodeLabel, EdgeWeight> GetEdges(const NodeLabel node) const;

        std::unordered_map<NodeLabel, std::unordered_map<NodeLabel, EdgeWeight>> node_map_;
};

template <typename NodeLabel, typename EdgeWeight>
Graph<NodeLabel, EdgeWeight>::Graph(void)
{
}

template <typename NodeLabel, typename EdgeWeight>
Graph<NodeLabel, EdgeWeight>::Graph(const Graph<NodeLabel, EdgeWeight> &graph) : node_map_(graph.node_map_)
{
}

template <typename NodeLabel, typename EdgeWeight>
Graph<NodeLabel, EdgeWeight> &Graph<NodeLabel, EdgeWeight>::operator=(const Graph<NodeLabel, EdgeWeight>& graph)
{
    node_map_ = graph.node_map_;
    return *this;
}

template <typename NodeLabel, typename EdgeWeight>
bool Graph<NodeLabel, EdgeWeight>::AddEdge(const NodeLabel start, const NodeLabel end, const EdgeWeight &weight)
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
bool Graph<NodeLabel, EdgeWeight>::RemoveEdge(const NodeLabel start, const NodeLabel end)
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

// Returns set of connected nodes
template <typename NodeLabel, typename EdgeWeight>
std::unordered_set<NodeLabel> Graph<NodeLabel, EdgeWeight>::BreadthFirstSearch(const NodeLabel start) const
{
    std::queue<NodeLabel>         node_queue;
    std::unordered_set<NodeLabel> visited_nodes;

    if (IsConnected(start))
    {
        node_queue.push(start);
        visited_nodes.insert(start);

        while (!node_queue.empty())
        {
            for (const typename std::unordered_map<NodeLabel, EdgeWeight>::value_type &edge : GetEdges(node_queue.front()))
            {
                if (visited_nodes.end() == visited_nodes.find(edge.first))
                {
                    visited_nodes.insert(edge.first);
                    node_queue.push(edge.first);
                }
            }

            node_queue.pop();
        }
    }

    return visited_nodes;
}

template <typename NodeLabel, typename EdgeWeight>
std::vector<NodeLabel> Graph<NodeLabel, EdgeWeight>::Dijkstra(const NodeLabel start, const NodeLabel end) const
{
    std::function<bool(const NodeDistance &distance_a, const NodeDistance &distance_b)> compare_distances = [](const NodeDistance &distance_a, const NodeDistance &distance_b)
                                                                                                            {
                                                                                                                return distance_a.distance > distance_b.distance;
                                                                                                            };
    std::priority_queue<NodeDistance, std::vector<NodeDistance>, decltype(compare_distances)> min_priority_queue(compare_distances);
    std::unordered_map<NodeLabel, NodeDistance>                                               node_distances;
    std::unordered_set<NodeLabel>                                                             visited_nodes;
    std::vector<NodeLabel>                                                                    path;
    NodeDistance                                                                              current_node_distance;

    current_node_distance.node     = start;
    current_node_distance.distance = static_cast<EdgeWeight>(0);
    min_priority_queue.emplace(start, static_cast<EdgeWeight>(0));
    node_distances[start] = current_node_distance;
    visited_nodes.insert(start);

    while (!min_priority_queue.empty())
    {
        current_node_distance = min_priority_queue.top();
        min_priority_queue.pop();

        if (end == current_node_distance.node)
        {
            break;
        }

        for (const typename std::unordered_map<NodeLabel, EdgeWeight>::value_type &edge : GetEdges(current_node_distance.node))
        {
            EdgeWeight distance = current_node_distance.distance + edge.second;

            if ((node_distances.end() == node_distances.find(edge.first)) ||
                (distance < node_distances[edge.first].distance))
            {
                NodeDistance node_distance(edge.first, current_node_distance.node, distance);

                node_distances[edge.first] = node_distance;

                if (visited_nodes.end() == visited_nodes.find(edge.first))
                {
                    visited_nodes.insert(edge.first);
                    min_priority_queue.push(node_distance);
                }
            }
        }
    }

    typename std::unordered_map<NodeLabel, NodeDistance>::const_iterator end_node_distance = node_distances.find(end);
    if (node_distances.end() != end_node_distance)
    {
        NodeDistance node_distance = end_node_distance->second;

        while (start != node_distance.node)
        {
            path.push_back(node_distance.node);
            node_distance = node_distances[node_distance.previous];
        }
        path.push_back(start);

        std::reverse(path.begin(), path.end());
    }

    return path;
}

template <typename NodeLabel, typename EdgeWeight>
bool Graph<NodeLabel, EdgeWeight>::IsConnected(const NodeLabel node) const
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
std::unordered_map<NodeLabel, EdgeWeight> Graph<NodeLabel, EdgeWeight>::GetEdges(const NodeLabel node) const
{
    std::unordered_map<NodeLabel, EdgeWeight>                                                         edges;
    typename std::unordered_map<NodeLabel, std::unordered_map<NodeLabel, EdgeWeight>>::const_iterator node_edges = node_map_.find(node);

    if (node_map_.end() != node_edges)
    {
        edges = node_edges->second;
    }

    return edges;
}

#endif // TRAINS_SRC_COMMON_INC_GRAPH_H