//
// Created by Jamie on 5/4/2020.
//

#include "HeuristicSearch.h"
#include <utility>

using namespace std;

int HeuristicSearch::heuristicScore(const State &a, const State &b, DisjointPatternDatabase & pdb) const {
    if (heuristicType == HAMMING) return hammingDistance(a, b);
    if (heuristicType == MANHATTAN) return manhattanDistance(a, b);
    if (heuristicType == LINEAR_CONFLICT) return linConfDistance(a, b);
    // Note: BoardN must be 5!!!
    if (heuristicType == D_PDB) return pdb.getHeuristic(a);
    return 0;
}

HeuristicSearch::~HeuristicSearch() {
    heuristicType = 0;
    closedList.clear();
    while (!pStk.empty()) pStk.pop();
    pSet.clear();
}

bool HeuristicSearch::AStarSearch(const State &start, const State &goal, DisjointPatternDatabase & pdb) {
    nExpanded = 0;
    maxDepth = 0;
    priority_queue<pair<int, State> > openList;
    map<State, int> gScore;
    int fScore;
    int tentativeGScore;
    State nullState;
    closedList[start] = nullState;
    gScore[start] = 0; fScore = heuristicScore(start, goal, pdb);
    openList.push({-fScore, start});
    while (!openList.empty()) {
        if (++nExpanded%100000 == 0) cout << openList.size() << endl;
        State current = openList.top().second;
        maxDepth = max(maxDepth, gScore[current]);
        openList.pop();

        if (current == goal) {
            return true;
        }

        if (maxDepth > LIMIT_DEPTH) {
            cout << "Height limit exceeded! @" << endl << current;
            break;
        }

        if (closedList.size() > NODE_LIMIT) {
            cout << "State limit exceeded! @" << endl << current;
            break;
        }

        // for each neighbor of current
        int zX = -1, zY = -1;
        State::getZeroPos(current, zX, zY);
        for (int i = 0; i < 4; ++i) {
            int zXNew = zX + dirX[i];
            int zYNew = zY + dirY[i];

            if (State::isValid(zXNew, zYNew)) {
                State neighbor = current;
                swap(neighbor.A[zX][zY], neighbor.A[zXNew][zYNew]);
                tentativeGScore = gScore[current] + 1;
                if ((gScore.find(neighbor) == gScore.end()) || (tentativeGScore < gScore[neighbor])) {
                    closedList[neighbor] = current;
                    gScore[neighbor] = tentativeGScore;
                    fScore = gScore[neighbor] + heuristicScore(neighbor, goal, pdb);
                    openList.push({-fScore, neighbor});
                }
            }
        }
    }
    nExpanded = closedList.size();
    return false;
}

int HeuristicSearch::hammingDistance(const State &a, const State &b) {
    int conflicts = 0;
    for (int i = 0; i < State::boardN; i++)
        for (int j = 0; j < State::boardN; j++)
            if (a.A[i][j] && a.A[i][j] != b.A[i][j])conflicts++;
    return conflicts;
}

int HeuristicSearch::manhattanDistance(const State &a, const State &b) {
    int sum = 0;
    iByte pR[(State::boardN * State::boardN) + 1];
    iByte pC[(State::boardN * State::boardN) + 1];
    for (int r = 0; r < State::boardN; r++) {
        for (int c = 0; c < State::boardN; c++) {
            pR[a.A[r][c]] = static_cast<iByte>(r);
            pC[a.A[r][c]] = static_cast<iByte>(c);
        }
    }
    for (int r = 0; r < State::boardN; r++)
        for (int c = 0; c < State::boardN; c++)
            if (b.A[r][c])
                sum += abs(pR[b.A[r][c]] - r) + abs(pC[b.A[r][c]] - c);
    return sum;
}

int HeuristicSearch::nLinConfDistance(const State &a, const State &b) {
    int conflicts = 0;
    iByte pR[(State::boardN * State::boardN) + 1];
    iByte pC[(State::boardN * State::boardN) + 1];
    for (int r = 0; r < State::boardN; r++) {
        for (int c = 0; c < State::boardN; c++) {
            pR[a.A[r][c]] = static_cast<iByte>(r);
            pC[a.A[r][c]] = static_cast<iByte>(c);
        }
    }

    // row conflicts - @checked_okay
    for (int r = 0; r < State::boardN; r++) {
        for (int cl = 0; cl < State::boardN; cl++) {
            for (int cr = cl + 1; cr < State::boardN; cr++) {
                if (b.A[r][cl] && b.A[r][cr] && r == pR[b.A[r][cl]] && pR[b.A[r][cl]] == pR[b.A[r][cr]] &&
                    pC[b.A[r][cl]] > pC[b.A[r][cr]]) {
                    conflicts++;
                }
            }
        }
    }

    // column conflicts -
    for (int c = 0; c < State::boardN; c++) {
        for (int rU = 0; rU < State::boardN; rU++) {
            for (int rD = rU + 1; rD < State::boardN; rD++) {
                if (b.A[rU][c] && b.A[rD][c] && c == pC[b.A[rU][c]] && pC[b.A[rU][c]] == pC[b.A[rD][c]] &&
                    pR[b.A[rU][c]] > pR[b.A[rD][c]]) {
                }
            }
        }
    }

    return conflicts;
}

int HeuristicSearch::linConfDistance(const State &a, const State &b) {
    return manhattanDistance(a, b) + 2 * nLinConfDistance(a, b);
}

void HeuristicSearch::printPath(const State &start, const State &goal) {
    std::vector<State> path;
    cout << "Getting path..." << endl;
    if (searchType == A_STAR) {
        getPathAStar(path, start, goal);
    } else {
        while (!pStk.empty()) path.push_back(pStk.top()), pStk.pop();
        reverse(path.begin(), path.end());
    }

    if (path.empty()) {
        cout << "Failed." << endl;
        return;
    } else {
        cout << "Fount path as follows:" << endl;
        for(auto & it : path) {
            cout << it;
        }
    }
}

bool HeuristicSearch::executeSearch(const State &start, const State &goal, DisjointPatternDatabase & pdb) {
    if (searchType == A_STAR) {
        return AStarSearch(start, goal, pdb);
    }
    else {
        return IDASearch(start, goal, pdb);
    }
}

bool HeuristicSearch::IDASearch(const State &start, const State &goal, DisjointPatternDatabase & pdb) {

    nExpanded = 0;
    maxDepth = 0;

    int bound = heuristicScore(start, goal, pdb);

    pStk.push(start);
    pSet.insert(start);

    int t;

    while (true) {
        t = idaRecursiveSearch(pStk, pSet, 0, bound, goal, pdb);
        if (t == FOUND) {
            maxDepth = (int) pSet.size() - 1;
            return true;
        }
        if (t == INT32_MAX) return false;
        bound = t;
    }
}

void HeuristicSearch::getPathAStar(vector<State> &path, const State& start, const State&goal) {
    path.clear();
    State current = goal;
    while (!closedList[current].isEmpty) {
        path.push_back(current);
        current = closedList[current];
    }
    reverse(path.begin(), path.end());
}

int HeuristicSearch::idaRecursiveSearch(stack<State> &path, std::set<State> &pathSet, int gScore, int bound,
        const State& goal, DisjointPatternDatabase & pdb) {
    State current = path.top();
    int fScore = gScore + heuristicScore(current, goal, pdb);

    if (fScore > bound) return fScore;

    if (current == goal) return FOUND;

    int min = INT32_MAX;

    int t;

    // for each neighbor of current
    int zX = -1, zY = -1;
    State::getZeroPos(current, zX, zY);
    for (int i = 0; i < 4; ++i) {
        int zXNew = zX + dirX[i];
        int zYNew = zY + dirY[i];

        if (State::isValid(zXNew, zYNew)) {
            State neighbor = current;
            swap(neighbor.A[zX][zY], neighbor.A[zXNew][zYNew]);
            if (pathSet.find(neighbor) == pathSet.end()) {
                path.push(neighbor);
                pathSet.insert(neighbor);
                nExpanded++;
                t = idaRecursiveSearch(path, pathSet, gScore + 1, bound, goal, pdb);
                if (t == FOUND) return t;
                if (t < min) min = t;
                pathSet.erase(path.top());
                path.pop();
            }
        }
    }

    int depth = (int) pSet.size() - 1;
    if (maxDepth < depth) {
        maxDepth = depth;
        cout << "Current max depth: " << maxDepth << ", nodes opened: " << nExpanded << endl;
    }
    return min;
}