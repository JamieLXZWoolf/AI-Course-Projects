//
// Created by Jamie on 5/4/2020.
//

#include "State.h"

using namespace std;

int State::boardN = 0;

bool State::isValid(int x, int y) {
    return x >= 0 && y >= 0 && x < State::boardN && y < State::boardN;
}

State::State() {
    isEmpty = true;
    A = new tile *[boardN];
    for (int i = 0; i < boardN; ++i) {
        A[i] = new tile[boardN];
        memset(A[i], 0, boardN * sizeof(tile));
    }
}

State::State(const State &state) {
    if (this != &state) {
//        this->~State();
        this->isEmpty = state.isEmpty;

        A = new tile *[boardN];

        for (int i = 0; i < boardN; ++i) {
            A[i] = new tile[boardN];
            for (int j = 0; j < boardN; ++j) {
                A[i][j] = state.A[i][j];
            }
        }
    }
}

State::~State() {
    if (A == nullptr || isEmpty) return;

    for (int i = 0; i < boardN; ++i) {
        delete A[i];
    }

    delete [] A;
    A = nullptr;
}

ostream &operator<<(std::ostream &os, const State &state) {
    if (!state.A) return os;

    for (int i = 0; i < State::boardN; ++i) {
        for (int j = 0; j < State::boardN; ++j) {
            if (state.A[i][j])
                os << setw(PRINT_W) << (static_cast<int>(state.A[i][j])) << " ";
            else
                os << setw(PRINT_W) << "  " << " ";
        }
        os << endl;
    }
    for (int k = 0; k < State::boardN; ++k) {
        os << "----";
    }
    os <<  endl;
    return os;
}

State &State::operator=(const State &state) {
    if (this != &state) {
        this->~State();
        this->isEmpty = state.isEmpty;

        A = new tile *[boardN];

        for (int i = 0; i < boardN; ++i) {
            A[i] = new tile[boardN];
            for (int j = 0; j < boardN; ++j) {
                A[i][j] = state.A[i][j];
            }
        }
    }

    return *this;
}

bool State::operator==(const State &right) const {
    if (this->isEmpty) {
        return right.isEmpty;
    }

    for (int i = 0; i < boardN; ++i) {
        for (int j = 0; j < boardN; ++j) {
            if (A[i][j] != right.A[i][j]) return false;
        }
    }

    return true;
}

bool State::operator!=(const State &right) const {
    return !(*this == right);
}

bool State::operator<(const State &right) const {
    if (this->isEmpty || right.isEmpty) {
        cout << "One or both of the operands is empty!" << endl;
        return false;
    }

    for (int i = 0; i < boardN; ++i) {
        for (int j = 0; j < boardN; ++j) {
            if (A[i][j] < right.A[i][j]) return true;
            else if (A[i][j] == right.A[i][j]) continue;
            else return false;
        }
    }
    return false;
}

bool State::isSolvable() const {
    unsigned blankPosition = 0;
    vector<int> arr;

    for (int i = 0; i < boardN; ++i) {
        for (int j = 0; j < boardN; ++j) {
            if(A[i][j]) arr.push_back(A[i][j]);
            else blankPosition = i;
        }
    }

    unsigned invCount = numInverted(arr);

    bool isOddBoard = static_cast<bool>(boardN & 1);

    return ((isOddBoard && !(invCount & 1)) || (!isOddBoard && ((blankPosition + invCount) & 1)));
}

int State::numInverted(const vector<int> &arr) {
    int inv_count = 0;
    for (int i = 0; i < arr.size() - 1; i++)
        for (int j = i + 1; j < arr.size(); j++)
            if (arr[i] > arr[j])
                inv_count++;

    return inv_count;
}

void State::getZeroPos(const State &state, int &zX, int &zY) {
    zX = zY = -1;
    for (int i = 0; i < State::boardN; i++) {
        for (int j = 0; j < State::boardN; j++)
            if (!state.A[i][j]) {
                zX = i, zY = j;
            }
    }
}