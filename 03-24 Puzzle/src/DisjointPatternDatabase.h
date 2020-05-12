//
// Created by Jamie on 5/10/2020.
//

#ifndef T4PUZZLE_DISJOINTPATTERNDATABASE_H
#define T4PUZZLE_DISJOINTPATTERNDATABASE_H

#include <map>
#include <bitset>
#include <fstream>
#include <string>
#include <algorithm>
#include <queue>
#include "State.h"

#define ALL_PERM 127512000

typedef std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<iByte>>>>>> v6;
typedef std::vector<std::vector<std::vector<std::vector<std::vector<iByte>>>>> v5;
typedef std::vector<std::vector<std::vector<std::vector<iByte>>>> v4;
typedef std::vector<std::vector<std::vector<iByte>>> v3;
typedef std::vector<std::vector<iByte>> v2;
typedef std::vector<iByte> v1;

class DisjointPatternDatabase {
public:
    v6 normalPds[4];
    v6 reversePds[4];

    static State goal;

    static std::vector<std::vector<iByte>> nmPs;

    static std::vector<std::vector<iByte>> rvPs;

    explicit DisjointPatternDatabase(bool is5);

    int getHeuristic(const State &s);

    static void save(v6 &pdbMap, const std::string& fileName);

    static void read(v6 &pdbMap, const std::string& fileName);

    static bool inVector(const std::vector<iByte> &vec, const iByte &t);

    static void getPDB(v6 & pdbMap, std::vector<iByte> &pdbNs);

    static void getNumPos(const State &state, int &zX, int &zY, iByte N);

    static std::vector<unsigned> getCord(const State & s, std::vector<iByte> &pdbNs);

    static void initV6(v6 &vec);
};

#endif //T4PUZZLE_DISJOINTPATTERNDATABASE_H
