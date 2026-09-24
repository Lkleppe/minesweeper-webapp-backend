#pragma once

#include <vector>
#include "Tile.h"
#include "Position.h"

class Board {
public:
    Board(int rows, int cols);

    Tile& getTile(const Position& pos);
    const Tile& getTile(const Position& pos) const;

    void placeMines(int mineCount, const Position& safePos);
    void calculateAdjacentMines();
    std::vector<Position> revealTile(const Position& pos);
    void flagTile(const Position& pos);

    bool isInBounds(const Position& pos) const;
    std::vector<Position> getAdjacentPositions(const Position& pos) const;

    int getRows() const;
    int getCols() const;
    int getMineCount() const;
    int getRevealedCount() const;

private:
    int rows;
    int cols;
    int mineCount;
    std::vector<std::vector<Tile>> grid;
};
