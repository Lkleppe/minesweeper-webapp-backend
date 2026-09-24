#include "Tile.h"

Tile::Tile() : state(TileState::Hidden), mine(false), adjacentMines(0) {}

void Tile::reveal() {
    state = TileState::Revealed;
}

void Tile::flag() {
    if (state == TileState::Hidden)
        state = TileState::Flagged;
}

void Tile::unflag() {
    if (state == TileState::Flagged)
        state = TileState::Hidden;
}

TileState Tile::getState() const { return state; }
bool Tile::isMine() const { return mine; }
int Tile::getAdjacentMines() const { return adjacentMines; }

void Tile::setMine(bool m) { mine = m; }
void Tile::setAdjacentMines(int count) { adjacentMines = count; }
