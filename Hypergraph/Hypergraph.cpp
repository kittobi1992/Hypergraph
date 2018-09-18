#include "Hypergraph.h"

#include <algorithm>

Hypergraph::Hypergraph(bool weightedNodes, bool weightedEdges)
{
    weightedNodes = weightedNodes;
    weightedEdges = weightedEdges;
}

template <class T>
void validateID(std::vector<T> v, int id) {
    if (std::find_if(v.begin(), v.end(), [id](T &n) {return n.id == id; }) != v.end()) {
        throw std::invalid_argument("Duplicate node id: " + id);
    }
}

void Hypergraph::addNode(int id, int weight)
{
    validateID(nodes, id);
    nodes.push_back(HNode(id, weight));
}


void Hypergraph::addEdge(int id, std::vector<int> nodeIds, int weight)
{
    validateID(edges, id);
    edges.push_back(HEdge(id, nodeIds, weight));
}


Hypergraph::HNode::HNode(int id, int weight)
{
    id = id;
    weight = weight;
}


Hypergraph::HEdge::HEdge(int id, std::vector<int> nodeIds, int weight)
{
    id = id;
    weight = weight;
    nodeIds = move(nodeIds);
}

void Hypergraph::exportToHMetis(std::ostream &os)
{
    /*
    The first line contains either two or three integers. The first integer is the number of hyperedges (|Eh|), the second
    is the number of vertices (|V|), and the third integer (fmt) contains information about the type of the hypergraph. In
    particular, depending on the value of fmt, the hypergraph H can have weights on either the hyperedges, the vertices,
    or both. In the case that H is unweighted (i.e., all the hyperedges and vertices have the same weight), fmt is omitted.
    */
    auto fmt = "";
    if (weightedEdges && weightedNodes) {
        fmt = "11";
    }
    else if (weightedNodes) {
        fmt = "10";
    }
    else if (weightedEdges) {
        fmt = "1";
    }
    os << edges.size() << nodes.size() << fmt << std::endl;

    /*
    After this first line, the remaining |Eh| lines store the vertices contained in each hyperedge–one line per hyperedge. In
    particular, the i th line (excluding comment lines) contains the vertices that are included in the (i−1)th hyperedge.
    The first integer in each line contains the weight of the respective hyperedge.
    */
    for (auto &edge : edges) {
        if (weightedEdges) {
            os << edge.weight;
        }
        for (auto &id : edge.nodeIds) {
            os << id;
        }
        os << std::endl;
    }

    /*
    If weights on nodes are enabled,| V | lines are appended to the input file, containing the weight of the | V | vertices.
    */
    if (weightedNodes) {
        std::sort(nodes.begin(), nodes.end(), [](HNode &n, HNode &m) {return n.id < m.id; });
        for (auto &node : nodes) {
            os << node.weight << std::endl;
        }
    }
}