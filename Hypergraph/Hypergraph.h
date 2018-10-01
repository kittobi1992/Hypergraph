#pragma once

#include <vector>
#include <iostream>

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

    std::vector<HNode> nodes;
    std::vector<HEdge> edges;

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
