//
// Created by Jamie on 5/10/2020.
//

#include "DisjointPatternDatabase.h"

using namespace std;

State DisjointPatternDatabase::goal;

std::vector<std::vector<iByte>> DisjointPatternDatabase::nmPs = {{1,  5,  6,  10, 11, 12},
                                                                 {2,  3,  4,  7,  8,  9},
                                                                 {15, 16, 17, 20, 21, 22},
                                                                 {13, 14, 18, 19, 23, 24}};

std::vector<std::vector<iByte>> DisjointPatternDatabase::rvPs = {{1,  2,  5,  6,  7,  12},
                                                                 {3,  4,  8,  9,  13, 14},
                                                                 {10, 11, 15, 16, 20, 21},
                                                                 {17, 18, 19, 22, 23, 24}};

DisjointPatternDatabase::DisjointPatternDatabase(bool is5) {
    if (is5) {
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
            initV6(reversePds[j]);
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

        } else {
            cout << "All PDBs exist, using saved checkpoints." << endl;
        }
    }
}

bool DisjointPatternDatabase::inVector(const vector<iByte> &vec, const iByte &t) {
    return find(vec.begin(), vec.end(), t) != vec.end();
}

void DisjointPatternDatabase::save(v6 &pdbMap, const string& fileName) {
    ofstream fout(fileName, ios::out | ios::binary);

    if (fout.is_open()) {
        cout << "Writing data to file <" << fileName << ">..." << endl;
        for (int i = 0; i < 25; ++i) {
            for (int j = 0; j < 25; ++j) {
                for (int k = 0; k < 25; ++k) {
                    for (int l = 0; l < 25; ++l) {
                        for (int m = 0; m < 25; ++m) {
                            for (int n = 0; n < 25; ++n) {
                                fout.write((char*) &pdbMap[i][j][k][l][m][n], sizeof(iByte));
                            }
                        }
                    }
                }
            }
        }
        fout.close();
    } else {
        cout << "File <" << fileName << "> opening failed!!!" << endl;
    }
}

void DisjointPatternDatabase::read(v6 &pdbMap, const string& fileName) {
    ifstream fin(fileName, ios::in | ios::binary);

    if (fin.is_open()) {
        cout << "Reading <" << fileName << ">..." << endl;
        string line;
        char buf[sizeof(iByte)];
        for (int i = 0; i < 25; ++i) {
            for (int j = 0; j < 25; ++j) {
                for (int k = 0; k < 25; ++k) {
                    for (int l = 0; l < 25; ++l) {
                        for (int m = 0; m < 25; ++m) {
                            for (int n = 0; n < 25; ++n) {
                                pdbMap[i][j][k][l][m][n] = (iByte) fin.get();
                            }
                        }
                    }
                }
            }
        }
        fin.close();
    } else {
        cout << "File <" << fileName << "> doesn't exist!!!" << endl;
    }
}

void DisjointPatternDatabase::getPDB(v6 &pdbMap, vector<iByte> &pdbNs) {
    State maskedGoal = goal;

    cout << goal;

    for (int i = 0; i < State::boardN; ++i) {
        for (int j = 0; j < State::boardN; ++j) {
            if (!(inVector(pdbNs, maskedGoal.A[i][j]) || !maskedGoal.A[i][j])) {
                maskedGoal.A[i][j] = -1;
            }
        }
    }

    cout << maskedGoal;

    queue<State> que;
    que.push(maskedGoal);

    cout << "Allocating 6-D bitset vector... ";
    // initialize a bitset that stores the visited state.
    vector<vector<vector<vector<vector<vector<bitset<25>>>>>>> visited;
    for (int i = 0; i < 25; ++i) {
        vector<vector<vector<vector<vector<bitset<25>>>>>> vec5;
        visited.push_back(vec5);
        for (int j = 0; j < 25; ++j) {
            vector<vector<vector<vector<bitset<25>>>>> vec4;
            visited[i].push_back(vec4);
            for (int k = 0; k < 25; ++k) {
                vector<vector<vector<bitset<25>>>> vec3;
                visited[i][j].push_back(vec3);
                for (int l = 0; l < 25; ++l) {
                    vector<vector<bitset<25>>> vec2;
                    visited[i][j][k].push_back(vec2);
                    for (int m = 0; m < 25; ++m) {
                        vector<bitset<25>> vec1;
                        visited[i][j][k][l].push_back(vec1);
                        for (int n = 0; n < 25; ++n) {
                            bitset<25> temp;
                            visited[i][j][k][l][m].push_back(temp);
                        }
                    }
                }
            }
        }
    }

    cout << "Done." << endl;

    vector<unsigned> c = getCord(maskedGoal, pdbNs), d;

    pdbMap[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]] = 0;

    int counter = 0;

    bool inPB;
    int cost;
//    iByte N;

    int zX = -1, zY = -1;
    getNumPos(maskedGoal, zX, zY, 0);
    int bitPosition = (zX*5+zY);

    visited[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]][bitPosition] = true;

    while (!que.empty()) {
        auto current = que.front();
        que.pop();
        d = getCord(current, pdbNs);
        // for each neighbor of current

//        for (int i = 0; i < 6; ++i) {
//            N = pdbNs[i];
        getNumPos(current, zX, zY, 0);
        for (int j = 0; j < 4; ++j) {
            int zXNew = zX + dirX[j];
            int zYNew = zY + dirY[j];
            if (State::isValid(zXNew, zYNew)) {
                State neighbor = current;
                bool isBlk = neighbor.A[zXNew][zYNew] == -1;
                swap(neighbor.A[zX][zY], neighbor.A[zXNew][zYNew]);
                c = getCord(neighbor, pdbNs);
                bitPosition = zXNew*5+zYNew;

                if (visited[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]][bitPosition]) continue;
                visited[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]][bitPosition] = true;
                que.push(neighbor);
                if (pdbMap[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]] == -1) {
                    if (++counter % 100000 == 0) cout << 100 * (double) counter/ (double) ALL_PERM << "% complete." << endl;
                    cost = pdbMap[d[0]][d[1]][d[2]][d[3]][d[4]][d[5]];
                    pdbMap[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]] = isBlk ? cost : cost + 1;
                }
            }
        }
//        }
    }
    cout << "completed." << endl;
}

void DisjointPatternDatabase::getNumPos(const State &state, int &zX, int &zY, iByte N) {
    zX = zY = -1;
    for (int i = 0; i < State::boardN; i++) {
        for (int j = 0; j < State::boardN; j++)
            if (state.A[i][j] == N) {
                zX = i, zY = j;
            }
    }
}

std::vector<unsigned> DisjointPatternDatabase::getCord(const State &s, vector<iByte> &pdbNs) {
    vector<unsigned> res;
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 5; ++j) {
            for (int k = 0; k < 5; ++k) {
                if (pdbNs[i] == s.A[j][k]) res.push_back(j*5 + k);
            }
        }
    }
    return res;
}

void DisjointPatternDatabase::initV6(v6 &vec) {
    for (int i = 0; i < 25; ++i) {
        v5 vec5;
        vec.push_back(vec5);
        for (int j = 0; j < 25; ++j) {
            v4 vec4;
            vec[i].push_back(vec4);
            for (int k = 0; k < 25; ++k) {
                v3 vec3;
                vec[i][j].push_back(vec3);
                for (int l = 0; l < 25; ++l) {
                    v2 vec2;
                    vec[i][j][k].push_back(vec2);
                    for (int m = 0; m < 25; ++m) {
                        v1 vec1;
                        vec[i][j][k][l].push_back(vec1);
                        for (int n = 0; n < 25; ++n) {
                            vec[i][j][k][l][m].push_back(-1);
                        }
                    }
                }
            }
        }
    }
}

int DisjointPatternDatabase::getHeuristic(const State &s) {
    int n1, n2, n3, n4, i1, i2, i3, i4;
    vector<unsigned> c1, c2, c3, c4, d1, d2 ,d3, d4;
    c1 = getCord(s, nmPs[0]), c2 = getCord(s, nmPs[1]), c3 = getCord(s, nmPs[2]), c4 = getCord(s, nmPs[3]);
    d1 = getCord(s, rvPs[0]), d2 = getCord(s, rvPs[1]), d3 = getCord(s, rvPs[2]), d4 = getCord(s, rvPs[3]);
    n1 = normalPds[0][c1[0]][c1[1]][c1[2]][c1[3]][c1[4]][c1[5]];
    n2 = normalPds[1][c2[0]][c2[1]][c2[2]][c2[3]][c2[4]][c2[5]];
    n3 = normalPds[2][c3[0]][c3[1]][c3[2]][c3[3]][c3[4]][c3[5]];
    n4 = normalPds[3][c4[0]][c4[1]][c4[2]][c4[3]][c4[4]][c4[5]];

    i1 = reversePds[0][d1[0]][d1[1]][d1[2]][d1[3]][d1[4]][d1[5]];
    i2 = reversePds[1][d2[0]][d2[1]][d2[2]][d2[3]][d2[4]][d2[5]];
    i3 = reversePds[2][d3[0]][d3[1]][d3[2]][d3[3]][d3[4]][d3[5]];
    i4 = reversePds[3][d4[0]][d4[1]][d4[2]][d4[3]][d4[4]][d4[5]];

    return max(n1 + n2 + n3 + n4, i1 + i2 + i3 + i4);
}
