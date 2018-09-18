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
    void addEdge(int id, std::vector<int> nodeIds, int weight = 0);

    void exportToHMetis(std::ostream &os);

};