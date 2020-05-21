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

typedef int8_t iByte;

const int dirX[4] = {0, 0, 1, -1}; // RIGHT-LEFT-DOWN-UP
const int dirY[4] = {1, -1, 0, 0}; // RIGHT-LEFT-DOWN-UP

class State {
public:
    // member variables

    static int boardN;

    bool isEmpty;

    iByte **A;

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

    static void getZeroPos(const State &state, int &zX, int &zY);

    static bool isValid(int x, int y);

    bool isSolvable(const State &goal) const;

private:

    static int numInverted(const std::vector<int> &arr);
};

#endif //T4PUZZLE_STATE_H
