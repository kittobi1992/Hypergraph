#include "pch.h"
#include "../Hypergraph/Hypergraph.cpp"
#include "../Hypergraph/Hypergraph.h"

class HypergraphTest : public ::testing::Test {

protected:

    virtual void SetUp() {
        hyper = std::unique_ptr<Hypergraph>(new Hypergraph());
    }

    std::unique_ptr<Hypergraph> hyper;
};

class NodeTest : public HypergraphTest {};
class EdgeTest : public HypergraphTest {};
class HMetisTest : public HypergraphTest {};

TEST_F(HypergraphTest, CorrectInit) {
    auto hyper_default = std::unique_ptr<Hypergraph>(new Hypergraph());
    EXPECT_FALSE(hyper_default->weightedNodes);
    EXPECT_FALSE(hyper_default->weightedEdges);

    auto hyper_false = std::unique_ptr<Hypergraph>(new Hypergraph(false, false));
    EXPECT_FALSE(hyper_false->weightedNodes);
    EXPECT_FALSE(hyper_false->weightedEdges);

    auto hyper_true = std::unique_ptr<Hypergraph>(new Hypergraph(true, true));
    EXPECT_TRUE(hyper_true->weightedNodes);
    EXPECT_TRUE(hyper_true->weightedEdges);

    auto hyper_mixed = std::unique_ptr<Hypergraph>(new Hypergraph(true, false));
    EXPECT_TRUE(hyper_mixed->weightedNodes);
    EXPECT_FALSE(hyper_mixed->weightedEdges);
}


TEST_F(NodeTest, addnweighted) {
    hyper->addNode(0);
}

TEST_F(NodeTest, addWeighted) {
    hyper->addNode(0, 1);
}

TEST_F(NodeTest, duplicate) {
    hyper->addNode(0, 1);
    EXPECT_THROW(hyper->addNode(0, 2), std::invalid_argument);
}


TEST_F(EdgeTest, addUnweighted) {
    hyper->addNode(1);
    hyper->addNode(2);

    hyper->addEdge(0, std::vector<int>{1, 2});
}


TEST_F(EdgeTest, addWeighted) {
    hyper->addNode(1);
    hyper->addNode(2);

    hyper->addEdge(0, std::vector<int>{1, 2}, 1);
}

TEST_F(EdgeTest, duplicate) {
    hyper->addNode(1);
    hyper->addNode(2);

    hyper->addEdge(0, std::vector<int>{1, 2}, 1);
    EXPECT_THROW(hyper->addEdge(0, std::vector<int>{2, 3}, 1), std::invalid_argument);
}

TEST_F(EdgeTest, addNotExsitingNode) {
    hyper->addNode(1);

    EXPECT_THROW(hyper->addEdge(0, std::vector<int>{1, 2}), std::invalid_argument);
}

TEST_F(NodeTest, removeExisting) {
    hyper->addNode(0);
    hyper->removeNode(0);

}

TEST_F(NodeTest, removeNotExisting) {
    EXPECT_THROW(hyper->removeNode(0), std::invalid_argument);
}


TEST_F(NodeTest, removePartOfEdge) {
    hyper->addNode(1);
    hyper->addNode(2);

    hyper->addEdge(0, std::vector<int>{1, 2}, 1);

    EXPECT_THROW(hyper->removeNode(1), std::invalid_argument);
}

TEST_F(EdgeTest, removeExisting) {
    hyper->addNode(1);
    hyper->addNode(2);

    hyper->addEdge(0, std::vector<int>{1, 2});
    hyper->removeEdge(0);

}

TEST_F(EdgeTest, removeNotExisting) {
    EXPECT_THROW(hyper->removeEdge(0), std::invalid_argument);
}

TEST_F(HMetisTest, unweighted) {
    hyper->addNode(1);
    hyper->addNode(2);
    hyper->addNode(3);
    hyper->addNode(4);
    hyper->addNode(5);
    hyper->addNode(6);
    hyper->addNode(7);


    hyper->addEdge(0, std::vector<int>{1, 2});
    hyper->addEdge(1, std::vector<int>{2, 3, 4});
    hyper->addEdge(2, std::vector<int>{5, 6, 4});
    hyper->addEdge(3, std::vector<int>{1, 7, 5, 6});

    std::stringstream stream;
    hyper->exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 \n"
        "1 2 \n"
        "2 3 4 \n"
        "5 6 4 \n"
        "1 7 5 6 \n"
    );

    EXPECT_EQ(expected, result);
}

TEST_F(HMetisTest, weightedEdges) {
    hyper->weightedEdges = true;

    hyper->addNode(1);
    hyper->addNode(2);
    hyper->addNode(3);
    hyper->addNode(4);
    hyper->addNode(5);
    hyper->addNode(6);
    hyper->addNode(7);


    hyper->addEdge(0, std::vector<int>{1, 2}, 2);
    hyper->addEdge(1, std::vector<int>{1, 7, 5, 6}, 3);
    hyper->addEdge(2, std::vector<int>{5, 6, 4}, 8);
    hyper->addEdge(3, std::vector<int>{2, 3, 4}, 7);

    std::stringstream stream;
    hyper->exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 1\n"
        "2 1 2 \n"
        "3 1 7 5 6 \n"
        "8 5 6 4 \n"
        "7 2 3 4 \n"
    );

    EXPECT_EQ(expected, result);
}

TEST_F(HMetisTest, weightedNodes) {
    hyper->weightedNodes = true;

    hyper->addNode(1, 5);
    hyper->addNode(2, 1);
    hyper->addNode(3, 8);
    hyper->addNode(4, 7);
    hyper->addNode(5, 3);
    hyper->addNode(6, 9);
    hyper->addNode(7, 3);


    hyper->addEdge(0, std::vector<int>{1, 2});
    hyper->addEdge(1, std::vector<int>{2, 3, 4});
    hyper->addEdge(2, std::vector<int>{5, 6, 4});
    hyper->addEdge(3, std::vector<int>{1, 7, 5, 6});

    std::stringstream stream;
    hyper->exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 10\n"
        "1 2 \n"
        "2 3 4 \n"
        "5 6 4 \n"
        "1 7 5 6 \n"
        "5\n"
        "1\n"
        "8\n"
        "7\n"
        "3\n"
        "9\n"
        "3\n"
    );

    EXPECT_EQ(expected, result);
}


TEST_F(HMetisTest, weightedNodesEdges) {
    hyper->weightedNodes = true;
    hyper->weightedEdges = true;

    hyper->addNode(1, 5);
    hyper->addNode(2, 1);
    hyper->addNode(3, 8);
    hyper->addNode(4, 7);
    hyper->addNode(5, 3);
    hyper->addNode(6, 9);
    hyper->addNode(7, 3);


    hyper->addEdge(0, std::vector<int>{1, 2}, 2);
    hyper->addEdge(1, std::vector<int>{1, 7, 5, 6}, 3);
    hyper->addEdge(2, std::vector<int>{5, 6, 4}, 8);
    hyper->addEdge(3, std::vector<int>{2, 3, 4}, 7);

    std::stringstream stream;
    hyper->exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 11\n"
        "2 1 2 \n"
        "3 1 7 5 6 \n"
        "8 5 6 4 \n"
        "7 2 3 4 \n"
        "5\n"
        "1\n"
        "8\n"
        "7\n"
        "3\n"
        "9\n"
        "3\n"
    );

    EXPECT_EQ(expected, result);
}