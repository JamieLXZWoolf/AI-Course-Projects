//
// Created by Jamie on 5/4/2020.
//

#include <chrono>
#include "HeuristicSearch.h"

using namespace std;

void executeSearch(const State &start, const State &goal,
                   int heuristic = LINEAR_CONFLICT, bool searchType = A_STAR, bool printSol = true);

int main() {
    int boardN = 5;
    cin >> boardN;
    State::boardN = boardN;

    State goal;
    for (int i = 0; i < boardN; ++i) {
        for (int j = 0; j < boardN; ++j) {
            goal.A[i][j] = static_cast<tile>(i * State::boardN + j + 1);
        }
    }
    goal.A[State::boardN - 1][State::boardN - 1] = 0;
    goal.isEmpty = false;

    State start;
    start.isEmpty = false;
    int x;
    for (int i = 0; i < boardN; ++i) {
        for (int j = 0; j < boardN; ++j) {
            cin >> x;
            start.A[i][j] = static_cast<tile>(x);
        }
    }

    cout << "Start: \n" << start;
    cout << "Goal: \n" << goal;
    cout << "Here are the heuristic scores of the start state node:" << endl;
    cout << "Manhattan Distance: " << HeuristicSearch::manhattanDistance(start, goal) << endl;
    cout << "Hamming Distance: " << HeuristicSearch::hammingDistance(start, goal) << endl;
    cout << "N Linear Conflicts: " << HeuristicSearch::linConfDistance(start, goal) << endl;

    if (!start.isSolvable()) {
        cout << "No solution!" << endl;
        return 0;
    }
    else {
//        executeSearch(start, goal, LINEAR_CONFLICT, IDA, true);
//        executeSearch(start, goal, MANHATTAN, IDA, false);
//        executeSearch(start, goal, HAMMING, IDA, false);
        executeSearch(start, goal, LINEAR_CONFLICT, A_STAR, true);
        executeSearch(start, goal, MANHATTAN, A_STAR, false);
        executeSearch(start, goal, HAMMING, A_STAR, false);
    }
}

void executeSearch(const State &start, const State &goal,
                   int heuristic, bool searchType, bool printSol) {
    auto *starSearch = new HeuristicSearch();
    starSearch->heuristicType = heuristic;
    starSearch->searchType = searchType;

    auto startTime = chrono::steady_clock::now();

    bool suc = starSearch->executeSearch(start, goal);
    if (printSol && suc) {
       starSearch->printPath(start, goal);
    }

    auto endTime = chrono::steady_clock::now();
    auto diff = endTime - startTime;

    cout << "No of Nodes Opened: " << starSearch->nExpanded << endl;
    cout << "Max Depth Reached: " << starSearch->maxDepth << endl;
    cout << "Execution Time: " << chrono::duration<double, milli>(diff).count() << "ms" << endl;
    cout << endl;
    fflush(stdout);
    delete starSearch;
}