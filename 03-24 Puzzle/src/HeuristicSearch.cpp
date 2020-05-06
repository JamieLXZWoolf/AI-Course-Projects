//
// Created by Jamie on 5/4/2020.
//

#include "HeuristicSearch.h"
#include <utility>

int dirX[4] = {0, 0, 1, -1}; // RIGHT-LEFT-DOWN-UP
int dirY[4] = {1, -1, 0, 0}; // RIGHT-LEFT-DOWN-UP

using namespace std;

struct less_than_key
{
    inline bool operator() (const Node& struct1, const Node& struct2)
    {
        return (struct1.f < struct2.f);
    }
};

bool HeuristicSearch::isValid(int x, int y) {
    return x >= 0 && y >= 0 && x < State::boardN && y < State::boardN;
}

double HeuristicSearch::heuristicScore(const State &a, const State &b) {
    if (heuristicType == HAMMING) return hammingDistance(a, b);
    if (heuristicType == MANHATTAN) return manhattanDistance(a, b);
    if (heuristicType == LINEAR_CONFLICT) return linConfDistance(a, b);
    return 0;
}

HeuristicSearch::~HeuristicSearch() {
    heuristicType = 0;
    closedList.clear();
}

bool HeuristicSearch::AStarSearch(const State &start, const State &goal) {
    nExpanded = 0;
    maxDepth = 0;
    vector<Node> openList;
    double heu = heuristicScore(start, goal);
    State nullState;
    Node startNode = {start, nullState, 0, heu, 0 + heu};
    openList.push_back(startNode);
    closedList.push_back(startNode);
    int counter = 0;

    while (!openList.empty()) {
        if (++counter%1000 == 0) cout << openList.size() << endl;
        auto itr = min_element(openList.begin(), openList.end());
        Node u = *itr;
        closedList.push_back(u);
        openList.erase(itr);
        ++nExpanded;
        maxDepth = max(maxDepth, u.g);

        if (u.state == goal) {
            return true;
        }

        if (maxDepth > LIMIT_DEPTH) {
            cout << "Height limit exceeded! @" << endl << u.state;
            break;
        }

        if (closedList.size() > NODE_LIMIT) {
            cout << "State limit exceeded! @" << endl << u.state;
            break;
        }

        int zX = -1, zY = -1;
        State::getZeroPos(u.state, zX, zY);

        for (int i = 0; i < 4; ++i) {
            int zXNew = zX + dirX[i];
            int zYNew = zY + dirY[i];

            if (isValid(zXNew, zYNew)) {
                State temp = u.state;
                swap(temp.A[zX][zY], temp.A[zXNew][zYNew]);
                heu = heuristicScore(temp, goal);
                Node v = {temp, u.state, u.g + 1, heu, u.g + 1 + heu};
                auto itC = find(closedList.begin(), closedList.end(), v);
                bool inClosed = itC != closedList.end();
                if (inClosed) continue;
                auto itO = find(openList.begin(), openList.end(), v);
                bool inOpened = itO != openList.end();
                if (!inOpened) openList.push_back(v);
                else {
                    Node actualNode = openList.at(distance(openList.begin(), itO));
                    if (v.g < actualNode.g) {
                        actualNode.g = v.g;
                        actualNode.f = v.f;
                        actualNode.parent = v.parent;
                    }
                }
            }
        }
    }

    nExpanded = closedList.size();
    return false;
}


double HeuristicSearch::hammingDistance(const State &a, const State &b) {
    int conflicts = 0;
    for (int i = 0; i < State::boardN; i++)
        for (int j = 0; j < State::boardN; j++)
            if (a.A[i][j] && a.A[i][j] != b.A[i][j])conflicts++;
    return conflicts;
}

double HeuristicSearch::manhattanDistance(const State &a, const State &b) {
    int sum = 0;
    tile pR[(State::boardN * State::boardN) + 1];
    tile pC[(State::boardN * State::boardN) + 1];
    for (int r = 0; r < State::boardN; r++) {
        for (int c = 0; c < State::boardN; c++) {
            pR[a.A[r][c]] = static_cast<tile>(r);
            pC[a.A[r][c]] = static_cast<tile>(c);
        }
    }
    for (int r = 0; r < State::boardN; r++)
        for (int c = 0; c < State::boardN; c++)
            if (b.A[r][c])
                sum += abs(pR[b.A[r][c]] - r) + abs(pC[b.A[r][c]] - c);
    return sum;
}

double HeuristicSearch::nLinConfDistance(const State &a, const State &b) {
    int conflicts = 0;
    tile pR[(State::boardN * State::boardN) + 1];
    tile pC[(State::boardN * State::boardN) + 1];
    for (int r = 0; r < State::boardN; r++) {
        for (int c = 0; c < State::boardN; c++) {
            pR[a.A[r][c]] = static_cast<tile>(r);
            pC[a.A[r][c]] = static_cast<tile>(c);
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

double HeuristicSearch::linConfDistance(const State &a, const State &b) {
    return manhattanDistance(a, b) + 2 * nLinConfDistance(a, b);
}

void HeuristicSearch::printPath(const State &start, const State &goal) {
    std::vector<State> path;
    cout << "Getting path..." << endl;
    int cost = getPath(path, start, goal);
    if (path.empty()) {
        cout << "Failed." << endl;
        return;
    }
    else {
        cout << "Cost:" << cost << endl;
        cout << "Fount path as follows:" << endl;
        for(auto & it : path) {
            cout << it;
        }
    }
}

bool HeuristicSearch::executeSearch(const State &start, const State &goal) {
    if (searchType == A_STAR) {
        return AStarSearch(start, goal);
    }
    else {
        return IDASearch(start, goal);
    }
}

bool HeuristicSearch::IDASearch(const State &start, const State &goal) {
    return true;
}

int HeuristicSearch::getPath(vector<State> &path, const State& start, const State&goal) {
    int cost = closedList.back().g;
    path.push_back(closedList.back().state);
    State parent = closedList.back().parent;

    for (auto i = closedList.rbegin(); i != closedList.rend(); i++) {
        if ((*i).state == parent && !((*i).state == start)) {
            path.push_back((*i).state);
            parent = (*i).parent;
        }
    }
    path.push_back(start);

    reverse(path.begin(), path.end());
    return cost;
}
