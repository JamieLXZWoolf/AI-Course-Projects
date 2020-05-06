//
// Created by Jamie on 5/4/2020.
//

#ifndef T4PUZZLE_STATE_H
#define T4PUZZLE_STATE_H

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>

#define PRINT_W 3

#define RIGHT 0
#define LEFT 1
#define DOWN 2
#define UP 3

typedef int8_t tile;

class State {
public:
    // member variables

    static int boardN;

    bool isEmpty;

    tile **A;

    // constructors and destructor

    State();

    State(const State &state);

    ~State();

    // overloaded operators

    friend std::ostream &operator<<(std::ostream &os, const State &state);

    State &operator=(const State &state);

    bool operator==(const State &right) const;

    bool operator!=(const State &right) const;

    bool operator<(const State &right) const;

    // utilities

    static bool getZeroPos(const State &state, int &zX, int &zY);

    bool isSolvable() const;

private:

    static int numInverted(const std::vector<int> &arr);
};

#endif //T4PUZZLE_STATE_H
