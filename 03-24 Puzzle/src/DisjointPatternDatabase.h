//
// Created by Jamie on 5/10/2020.
//

#ifndef T4PUZZLE_DISJOINTPATTERNDATABASE_H
#define T4PUZZLE_DISJOINTPATTERNDATABASE_H

#include <map>
#include <fstream>
#include <string>
#include <algorithm>
#include <queue>
#include "State.h"

typedef tile v6[25][25][25][25][25][25];

class DisjointPatternDatabase {
public:
    v6 normalPds[4];
    v6 reversePds[4];

    static State goal;

    static std::vector<std::vector<tile>> nmPs;

    static std::vector<std::vector<tile>> rvPs;

    DisjointPatternDatabase();

    static void save(v6 &pdbMap, const std::string& fileName);

    static void read(v6 &pdbMap, const std::string& fileName);

    static bool inVector(const std::vector<tile> &vec, const tile &t);

    static void getPDB(v6 & pdbMap, std::vector<tile> &pdbNs);

    static std::string encodeCord(std::vector<unsigned> & cord);

    static std::vector<unsigned> decodeCord(const std::string &code);

    static void getNumPos(const State &state, int &zX, int &zY, tile N);

    static std::vector<unsigned> getCord(const State & s, std::vector<tile> &pdbNs);

    static void initV6(v6 &);
};

#endif //T4PUZZLE_DISJOINTPATTERNDATABASE_H
