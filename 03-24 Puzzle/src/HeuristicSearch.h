//
// Created by Jamie on 5/4/2020.
//

#ifndef T4PUZZLE_HEURISTICSEARCH_H
#define T4PUZZLE_HEURISTICSEARCH_H

#include <algorithm>
#include <queue>
#include <map>
#include <set>
#include <stack>
#include "State.h"

#define MANHATTAN 1
#define HAMMING 2
#define LINEAR_CONFLICT 3

#define LIMIT_DEPTH 100
#define NODE_LIMIT 4000000

#define A_STAR false
#define IDA true

#define FOUND -1
#define MAX_DOUBLE 1.79769e+308

class HeuristicSearch {
public:

    int maxDepth = 0;

    int nExpanded = 0;

    int heuristicType = MANHATTAN;

    bool searchType = A_STAR;

    std::map<State, State> closedList;

    std::stack<State> pStk;

    std::set<State> pSet;

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

    double idaRecursiveSearch(std::stack<State> &path, std::set<State> &pathSet,int gScore, double bound, const State& goal);

    void getPathAStar(std::vector<State> & path, const State& start, const State&goal);
};


#endif //T4PUZZLE_HEURISTICSEARCH_H
