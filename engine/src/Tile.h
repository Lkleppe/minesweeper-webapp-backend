#pragma once

enum class TileState {
    Hidden,
    Revealed,
    Flagged
};

class Tile {
public:
    Tile();

    void reveal();
    void flag();
    void unflag();

    TileState getState() const;
    bool isMine() const;
    int getAdjacentMines() const;

    void setMine(bool mine);
    void setAdjacentMines(int count);

private:
    TileState state;
    bool mine;
    int adjacentMines;
};
