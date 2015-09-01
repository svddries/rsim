#ifndef ERA_GRAPH_H_
#define ERA_GRAPH_H_

#include "era/datastructures/array_set.h"

#include <iostream>

namespace era
{

template<typename N, typename E>
class Graph
{

public:

    Graph() {}

    ~Graph() {}

    Id addNode(const N& n)
    {
        return nodes_.add(Node(n));
    }

    N& node(const Id& id) { return nodes_[id].value; }

    const N& node(const Id& id) const { return nodes_[id].value; }

    void removeNode(const Id& id)
    {
        const Node& n = nodes_[id];

        for(const auto& edge_id : n.child_edge_ids)
        {
            const Edge& e = edges_[edge_id];
            nodes_[e.child].removeParentEdge(e);
        }

        for(const auto& edge_id : n.parent_edge_ids)
        {
            const Edge& e = edges_[edge_id];
            nodes_[e.parent].removeChildEdge(e);
        }

        nodes_.remove(id);
    }

    Id addEdge(const Id& n1, const Id& n2, const E& e)
    {
        // TODO: do not add edge if it already exists

        Id edge_id = edges_.add(Edge(e, n1, n2,
                                     nodes_[n1].child_edge_ids.size(),
                                     nodes_[n2].parent_edge_ids.size()
                                     ));

        nodes_[n1].child_edge_ids.push_back(edge_id);
        nodes_[n2].parent_edge_ids.push_back(edge_id);
        return edge_id;
    }

    void print() const
    {
        for(const Node& n : nodes_)
        {
            std::cout << n.value << std::endl;

            for(const Id& edge_id : n.child_edge_ids)
            {
                const Edge& e = edges_[edge_id];
                std::cout << "    -- " << e.value << " -> " << nodes_[e.child].value << std::endl;
            }
        }
    }

private:

    struct Edge
    {
        Edge() {}
        Edge(const E& value_, const Id& parent_, const Id& child_, uint64_t edge_idx_parent_, uint64_t edge_idx_child_)
            : value(value_), parent(parent_), child(child_), edge_idx_parent(edge_idx_parent_), edge_idx_child(edge_idx_child_) {}

        E value;
        Id parent;
        Id child;

        // The index of this edge in the parent list
        uint64_t edge_idx_parent;

        // The index of this edge in the child list
        uint64_t edge_idx_child;
    };

    struct Node
    {
        Node() {}
        Node(const N& value_) : value(value_) {}

        void removeParentEdge(const Edge& e)
        {
            parent_edge_ids[e.edge_idx_child] = parent_edge_ids.back();
            parent_edge_ids.pop_back();
        }

        void removeChildEdge(const Edge& e)
        {
            child_edge_ids[e.edge_idx_parent] = child_edge_ids.back();
            child_edge_ids.pop_back();
        }

        N value;

        // Ids of the edges for which this node is the parent
        std::vector<Id> parent_edge_ids;

        // Ids of the edges for which this node is the child
        std::vector<Id> child_edge_ids;
    };

    ArraySet<Node> nodes_;

    ArraySet<Edge> edges_;

};

} // end namespace era

#endif
