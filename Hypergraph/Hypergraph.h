#pragma once

#include <vector>
#include <map>
#include <iostream>

// comment: 
// using NodeId = int;
// using NodeWeight = int;
// using HyperedgeId = int;
// using HyperedgeWeight = int;
// using HyperedgeVector = std::vector< NodeId >;
// Looks a bit stupid, but it will make your code much cleaner and better readable, if you use these types. You will heavily find
// in our codebase.

class Hypergraph
{
    struct HNode {
        int id;
        int weight;

        HNode(int id, int weight);
    };


    struct HEdge {
        int id;
        int weight;
        std::vector<int> nodeIds;

        HEdge(int id, std::vector<int> nodeIds, int weight);
    };
 
    // comment: I try to prevent the usage of std::map as often as I can because they are very slow in practice.
    // However, I think for your use case it is okay, because you only need the structure for modelling the hypergraph
    // and converting it to hMEtis-File-Format (no performance critical operations). Also you can use the partition id 
    // of a database table as node id ( advantage of 1-to-1 mapping between partition id and node ids )
    std::map<int, HNode> nodes;
    std::map<int, HEdge> edges;

public:
    bool weightedNodes;
    bool weightedEdges;

    Hypergraph(bool weightedNodes = false, bool weightedEdges = false);

    void addNode(int id, int weight = 0);
    // Nodes that are part of an edge must be added before the edge can be created.
    void addEdge(int id, std::vector<int> nodeIds, int weight = 0);

    // Before a node can be removed, all edges containing this node must be removed.
    void removeNode(int id);
    void removeEdge(int id);

    void exportToHMetis(std::ostream &os);

};
