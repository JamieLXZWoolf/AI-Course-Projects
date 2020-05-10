//
// Created by Jamie on 5/10/2020.
//

#include "DisjointPatternDatabase.h"

using namespace std;

State DisjointPatternDatabase::goal;

std::vector<std::vector<tile>> DisjointPatternDatabase::nmPs = {{1, 5, 6, 10, 11, 12},
                                                                {2, 3, 4, 7, 8, 9},
                                                                {15, 16, 17, 20, 21, 22},
                                                                {13, 14, 18, 19, 23, 24}};

std::vector<std::vector<tile>> DisjointPatternDatabase::rvPs = {{1, 2, 5, 6, 7, 12},
                                                                {3, 4, 8, 9, 13, 14},
                                                                {10, 11, 15, 16, 20, 21},
                                                                {17, 18, 19, 22, 23, 24}};

DisjointPatternDatabase::DisjointPatternDatabase() {
    bool pdbExist = true;
    for (int i = 0; i < 4; ++i) {
        initV6(normalPds[i]);
        ofstream tmpStm;
        string fn = "nmp" + to_string(i) + ".txt";
        tmpStm.open(fn, ios::in);
        if (!tmpStm.is_open()) {
            tmpStm.close();
            pdbExist = false;
            getPDB(normalPds[i], nmPs[i]);
            save(normalPds[i], fn);
        }
        else tmpStm.close(), read(normalPds[i], fn);
    }
    for (int j = 0; j < 4; ++j) {
        initV6(normalPds[j]);
        ofstream tmpStm;
        string fn = "rvp" + to_string(j) + ".txt";
        tmpStm.open(fn, ios::in);
        if (!tmpStm.is_open()) {
            tmpStm.close();
            pdbExist = false;
            getPDB(reversePds[j], rvPs[j]);
            save(reversePds[j], fn);
        }
        else tmpStm.close(), read(reversePds[j], fn);
    }
    if (!pdbExist) {
        cout << "At least one of the PDBs doesn't already exist. Created new ones." << endl;
        for (int i = 0; i < 4; ++i) {
        }

    } else {
        cout << "All PDBs exist, using saved checkpoints." << endl;
    }
}

bool DisjointPatternDatabase::inVector(const vector<tile> &vec, const tile &t) {
    return find(vec.begin(), vec.end(), t) != vec.end();
}

void DisjointPatternDatabase::save(v6 &pdbMap, const string& fileName) {
    ofstream file(fileName);

    if (file.is_open()) {
        for (int i = 0; i < 25; ++i) {
            for (int j = 0; j < 25; ++j) {
                for (int k = 0; k < 25; ++k) {
                    for (int l = 0; l < 25; ++l) {
                        for (int m = 0; m < 25; ++m) {
                            for (int n = 0; n < 25; ++n) {
                                vector<unsigned> cords;
                                cords.push_back(i), cords.push_back(j), cords.push_back(k), cords.push_back(l),
                                cords.push_back(m), cords.push_back(n);
                                string t;
                                t+= encodeCord(cords);
                                t+= "\t";
                                t+= to_string(pdbMap[i][j][k][l][m][n]);
                                t+= "\n";
                                file << t;
                            }
                        }
                    }
                }
            }
        }
        file.close();
    } else {
        cout << "File <" << fileName << "> opening failed!!!" << endl;
    }
}

void DisjointPatternDatabase::read(v6 &pdbMap, const string& fileName) {
    ifstream file(fileName);

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            auto pos = line.find('\t');
            string code = line.substr(0, pos);
            unsigned hs = stoi(line.substr(pos + 1));
            vector<unsigned> c = decodeCord(code);
            pdbMap[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]] = hs;
        }
        file.close();
    } else {
        cout << "File <" << fileName << "> doesn't exist!!!" << endl;
    }
}

void DisjointPatternDatabase::getPDB(v6 &pdbMap, vector<tile> &pdbNs) {
    State maskedGoal = goal;

    cout << goal;

    for (int i = 0; i < State::boardN; ++i) {
        for (int j = 0; j < State::boardN; ++j) {
            if (!inVector(pdbNs, maskedGoal.A[i][j])) {
                maskedGoal.A[i][j] = -1;
            }
        }
    }

    cout << maskedGoal;

    queue<State> que;
    que.push(maskedGoal);

    vector<unsigned> c = getCord(maskedGoal, pdbNs), d;

    pdbMap[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]] = 0;

    int counter = 0;

    bool inPB;
    unsigned cost;
    tile N = pdbNs[0];

    while (!que.empty()) {
        if (++counter % 10000 == 0) cout << counter << endl;

        auto current = que.front();
        que.pop();

        // for each neighbor of current
        int zX = -1, zY = -1;
        getNumPos(current, zX, zY, N);
        for (int i = 0; i < 4; ++i) {
            int zXNew = zX + dirX[i];
            int zYNew = zY + dirY[i];

            if (State::isValid(zXNew, zYNew)) {
                State neighbor = current;
                inPB = neighbor.A[zXNew][zYNew] != -1;
                swap(neighbor.A[zX][zY], neighbor.A[zXNew][zYNew]);

                c = getCord(neighbor, pdbNs);
                if (pdbMap[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]] == -1) {
                    d = getCord(current, pdbNs);
                    cost = pdbMap[d[0]][d[1]][d[2]][d[3]][d[4]][d[5]];
                    pdbMap[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]] = inPB ? cost + 1 : cost;
                    que.push(neighbor);
                }
            }
        }
    }
}

std::string DisjointPatternDatabase::encodeCord(vector<unsigned int> &cord) {
    string res;
    for (auto i : cord) res += to_string(i), res += " ";
    return res;
}

std::vector<unsigned> DisjointPatternDatabase::decodeCord(const string &code) {
    vector<unsigned> cords;

    for (auto i : code) {
        cords.push_back((unsigned) i - 48);
    }

    return cords;
}

void DisjointPatternDatabase::getNumPos(const State &state, int &zX, int &zY, tile N) {
    zX = zY = -1;
    for (int i = 0; i < State::boardN; i++) {
        for (int j = 0; j < State::boardN; j++)
            if (state.A[i][j] == N) {
                zX = i, zY = j;
            }
    }
}

std::vector<unsigned> DisjointPatternDatabase::getCord(const State &s, vector<tile> &pdbNs) {
    vector<unsigned> res;
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 5; ++j) {
            for (int k = 0; k < 5; ++k) {
                if (pdbNs[i] == s.A[j][k]) res.push_back(j * 5 + k);
            }
        }
    }
    return res;
}

void DisjointPatternDatabase::initV6(v6 &ar6) {
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            for (int k = 0; k < 25; ++k) {
                for (int l = 0; l < 25; ++l) {
                    for (int m = 0; m < 25; ++m) {
                        for (int n = 0; n < 25; ++n) {
                            ar6[i][j][k][l][m][n] = -1;
                        }
                    }
                }
            }
        }
    }
}
