//
// Created by Jamie on 5/4/2020.
//

#include <chrono>
#include "HeuristicSearch.h"

using namespace std;

void executeSearch(DisjointPatternDatabase &pdb, const State &start, const State &goal,
                   int heuristic = LINEAR_CONFLICT, bool searchType = A_STAR, bool printSol = true);

int main() {
    int boardN = 5;
    cin >> boardN;
    State::boardN = boardN;

    State goal;

    for (int i = 0; i < boardN; ++i) {
        for (int j = 0; j < boardN; ++j) {
            goal.A[i][j] = static_cast<iByte>(i * State::boardN + j);
        }
    }
    goal.isEmpty = false;

    // create and save / load pattern databases
    DisjointPatternDatabase::goal= goal;
    DisjointPatternDatabase pdb(State::boardN == 5);
    while (true) {
        State start;
        start.isEmpty = false;
        int x;
        for (int i = 0; i < boardN; ++i) {
            for (int j = 0; j < boardN; ++j) {
                cin >> x;
                if (x == -1) return 0;
                start.A[i][j] = static_cast<iByte>(x);
            }
        }

        cout << "Start: \n" << start;
        cout << "Goal: \n" << goal;
        cout << "Here are the heuristic scores of the start state node:" << endl;

        if (State::boardN == 5) cout << "Disjoint pattern database " << pdb.getHeuristic(start) << endl;
        cout << "Manhattan Distance: " << HeuristicSearch::manhattanDistance(start, goal) << endl;
        cout << "Hamming Distance: " << HeuristicSearch::hammingDistance(start, goal) << endl;
        cout << "N Linear Conflicts: " << HeuristicSearch::linConfDistance(start, goal) << endl;

        if (!start.isSolvable()) {
            cout << "No solution!" << endl;
        }
        else {
            cout << "IDA:" << endl;
            if (State::boardN == 5) {
                cout << "Disjoint Pattern Database:" << endl;
                executeSearch(pdb, start, goal, D_PDB, IDA, true);
            }
            cout << "Linear Conflict:" << endl;
            executeSearch(pdb, start, goal, LINEAR_CONFLICT, IDA, true);
            cout << "Manhattan:" << endl;
            executeSearch(pdb, start, goal, MANHATTAN, IDA, false);
            cout << "Hamming:" << endl;
            executeSearch(pdb, start, goal, HAMMING, IDA, false);
            cout << "A*:" << endl;
            cout << "Linear Conflict:" << endl;
            executeSearch(pdb, start, goal, LINEAR_CONFLICT, A_STAR, true);
            cout << "Manhattan:" << endl;
            executeSearch(pdb, start, goal, MANHATTAN, A_STAR, false);
            cout << "Hamming:" << endl;
            executeSearch(pdb, start, goal, HAMMING, A_STAR, false);
        }
    }
}

void executeSearch(DisjointPatternDatabase &pdb, const State &start, const State &goal,
                   int heuristic, bool searchType, bool printSol) {
    auto *starSearch = new HeuristicSearch();
    starSearch->heuristicType = heuristic;
    starSearch->searchType = searchType;

    auto startTime = chrono::steady_clock::now();

    bool suc = starSearch->executeSearch(start, goal, pdb);
    if (suc) {
        if (printSol) {
            starSearch->printPath(start, goal);
        }
        cout << "Solution Depth: " << starSearch->maxDepth << endl;
        cout << "No. of Nodes Opened: " << starSearch->nExpanded << endl;
    }

    auto endTime = chrono::steady_clock::now();
    auto diff = endTime - startTime;

    cout << "Execution Time: " << chrono::duration<double, milli>(diff).count() << "ms" << endl;
    cout << endl;
    fflush(stdout);
    delete starSearch;
}