#include "Hypergraph.h"

#include <algorithm>

// comment: In general, it is good that you think about invalid states where exceptions are thrown.
// But in your case I would prefer assertions instead of exceptions. This has several advantages:
// 1. Performance -> Assertions can be enabled in a debug build and disabled in release build, which
//    improve performance in release versions.
// 2. More clean code
// 3. Exceptions are thrown when user uses wrong input, but you can verify with gtests a correct usage
//    of your hypergraph model. Therefore, assertions are preferable.

template <class T>
void removeID(std::map<int, T> &v, int id) {
    auto it = v.find(id);
    if (it == v.end()) {
        throw std::invalid_argument("Not existing id: " + id);
    }
    else {
        v.erase(it);
    }
}

Hypergraph::Hypergraph(bool weightedNodes, bool weightedEdges)
{
    this->weightedNodes = weightedNodes;
    this->weightedEdges = weightedEdges;
}


void Hypergraph::addNode(int id, int weight)
{
    // comment: Make it more clear what is meant with this condition
    // => bool succes = nodes.insert(std::pair<int, HNode>(id, HNode(id, weight))).second
    //    if ( !success ) ...
    if (!nodes.insert(std::pair<int, HNode>(id, HNode(id, weight))).second) {
        throw std::invalid_argument("Duplicate id: " + id);
    }
}

void Hypergraph::addEdge(int id, std::vector<int> nodeIds, int weight)
{

    if (nodeIds.empty()) {
        throw std::invalid_argument("Edges with no containing nodes are not allowed.");
    }
    for (int nId : nodeIds) {
        if (nodes.find(nId) == nodes.end()) {
            throw std::invalid_argument("Node contained by edge must be added before.");
        }
    }
    if (!edges.insert(std::pair<int, HEdge>(id, HEdge(id, std::move(nodeIds), weight))).second) {
        throw std::invalid_argument("Duplicate id: " + id);
    }
    // comment: Another case, what is if you add the same hyperedge twice with different ids?
    // To fastly check if two hyperedges are the same (contains the same node ids) we use a so called footprint of
    // a hyperedge. A footprint is a hash function defined (in your case) over std::vector<int>. It is also described
    // in a KaHyPar paper ( might be the recursive bisection or direct k-way paper ). If two hyperedges are equal, they have the same
    // footprint. To implement this you need a extra mapping of Footprint -> Hyperedge Id
}

void Hypergraph::removeNode(int id)
{
    for (const auto& entry : edges) {
        auto &edge = entry.second;
        if (std::find(edge.nodeIds.begin(), edge.nodeIds.end(), id) != edge.nodeIds.end()) {
            // comment: Why? Just swap the id to end of the hyperedge vector and pop it => removal in O(1)
            throw std::invalid_argument("Cannot remove node that is part of existing edge.");
        }
    }
    removeID(nodes, id);
}

void Hypergraph::removeEdge(int id)
{
    removeID(edges, id);
}


Hypergraph::HNode::HNode(int id, int weight)
{
    this->id = id;
    this->weight = weight;
}


Hypergraph::HEdge::HEdge(int id, std::vector<int> nodeIds, int weight)
{
    this->id = id;
    this->weight = weight;
    this->nodeIds = move(nodeIds);
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
    os << edges.size() << " " << nodes.size() << " " << fmt << std::endl;

    /*
    After this first line, the remaining |Eh| lines store the vertices contained in each hyperedge–one line per hyperedge. In
    particular, the i th line (excluding comment lines) contains the vertices that are included in the (i−1)th hyperedge.
    The first integer in each line contains the weight of the respective hyperedge.
    */
    for (auto &entry : edges) {
        auto &edge = entry.second;

        if (weightedEdges) {
            os << edge.weight << " ";
        }
        for (auto &id : edge.nodeIds) {
            // comment: hMetis-File-Format requires that the ids of the nodes are numbered from 0..|V|-1.
            // Your code thus not guarantees that, because you use a std::map. In that case you have to compute
            // a mapping from your node ids to [0,|V|-1]
            os << id << " ";
        }
        os << std::endl;
    }

    /*
    If weights on nodes are enabled,| V | lines are appended to the input file, containing the weight of the | V | vertices.
    */
    if (weightedNodes) {
        for (auto &entry : nodes) {
            auto &node = entry.second;

            os << node.weight << std::endl;
        }
    }
}

