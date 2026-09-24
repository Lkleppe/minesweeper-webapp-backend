#include "Board.h"

#include <algorithm>
#include <queue>
#include <random>

Board::Board(int rows, int cols)
    : rows(rows), cols(cols), mineCount(0),
      grid(rows, std::vector<Tile>(cols)) {}

Tile& Board::getTile(const Position& pos) {
    return grid[pos.row][pos.col];
}

const Tile& Board::getTile(const Position& pos) const {
    return grid[pos.row][pos.col];
}

bool Board::isInBounds(const Position& pos) const {
    return pos.row >= 0 && pos.row < rows && pos.col >= 0 && pos.col < cols;
}

std::vector<Position> Board::getAdjacentPositions(const Position& pos) const {
    std::vector<Position> result;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            Position neighbor{pos.row + dr, pos.col + dc};
            if (isInBounds(neighbor))
                result.push_back(neighbor);
        }
    }
    return result;
}

void Board::placeMines(int count, const Position& safePos) {
    mineCount = count;

    std::vector<Position> candidates;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Position p{r, c};
            if (!(p == safePos))
                candidates.push_back(p);
        }
    }

    std::mt19937 rng(std::random_device{}());
    std::shuffle(candidates.begin(), candidates.end(), rng);

    for (int i = 0; i < count && i < static_cast<int>(candidates.size()); ++i)
        grid[candidates[i].row][candidates[i].col].setMine(true);
}

void Board::calculateAdjacentMines() {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c].isMine()) continue;
            int count = 0;
            for (const auto& neighbor : getAdjacentPositions({r, c})) {
                if (grid[neighbor.row][neighbor.col].isMine()) ++count;
            }
            grid[r][c].setAdjacentMines(count);
        }
    }
}

std::vector<Position> Board::revealTile(const Position& pos) {
    std::vector<Position> revealed;

    Tile& start = getTile(pos);
    if (start.getState() != TileState::Hidden) return revealed;

    start.reveal();
    revealed.push_back(pos);

    if (start.isMine() || start.getAdjacentMines() > 0) return revealed;

    std::queue<Position> queue;
    queue.push(pos);

    while (!queue.empty()) {
        Position current = queue.front();
        queue.pop();

        for (const auto& neighbor : getAdjacentPositions(current)) {
            Tile& tile = getTile(neighbor);
            if (tile.getState() != TileState::Hidden) continue;
            tile.reveal();
            revealed.push_back(neighbor);
            if (!tile.isMine() && tile.getAdjacentMines() == 0)
                queue.push(neighbor);
        }
    }

    return revealed;
}

void Board::flagTile(const Position& pos) {
    Tile& tile = getTile(pos);
    if (tile.getState() == TileState::Flagged)
        tile.unflag();
    else if (tile.getState() == TileState::Hidden)
        tile.flag();
}

int Board::getRows() const { return rows; }
int Board::getCols() const { return cols; }
int Board::getMineCount() const { return mineCount; }

int Board::getRevealedCount() const {
    int count = 0;
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            if (grid[r][c].getState() == TileState::Revealed) ++count;
    return count;
}
