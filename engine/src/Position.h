#pragma once

struct Position {
    int row;
    int col;

    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;
};
