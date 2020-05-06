//
// Created by Jamie on 5/4/2020.
//

#ifndef T4PUZZLE_HEURISTICSEARCH_H
#define T4PUZZLE_HEURISTICSEARCH_H

#include <algorithm>
#include "State.h"

#define MANHATTAN 1
#define HAMMING 2
#define LINEAR_CONFLICT 3

#define LIMIT_DEPTH 600
#define NODE_LIMIT 3000000

#define A_STAR false
#define IDA true

struct Node {
    State state;
    State parent;
    int g;
    double h, f;

    bool operator==(const Node &rhs) const {
        return state == rhs.state;
    }

    bool operator==(const State &right) const {
        return state == right;
    }

    bool operator<(const Node &rhs) const {
        return f < rhs.f;
    }

    bool operator!=(const Node &rhs) const {
        return !(rhs == *this);
    }
};


class HeuristicSearch {
public:

    int maxDepth = 0;

    int nExpanded = 0;

    int heuristicType = MANHATTAN;

    bool searchType = A_STAR;

    std::vector<Node> closedList;

    // destructor

    virtual ~HeuristicSearch();

    // member functions

    bool AStarSearch(const State &start, const State &goal);

    bool IDASearch(const State &start, const State &goal);

    bool executeSearch(const State &start, const State &goal);

    double heuristicScore(const State &a, const State &b);

    static double hammingDistance(const State &a, const State &b);

    static double manhattanDistance(const State &a, const State &b);

    static double nLinConfDistance(const State &a, const State &b);

    static double linConfDistance(const State &a, const State &b);

    void printPath(const State &start, const State &goal);

private:

    // utilities

    static bool isValid(int x, int y);

    int getPath(std::vector<State> & path, const State& start, const State&goal);
};


#endif //T4PUZZLE_HEURISTICSEARCH_H
