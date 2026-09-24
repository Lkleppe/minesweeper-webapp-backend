#pragma once

#include <vector>
#include "Board.h"
#include "GameConfig.h"
#include "Position.h"

enum class GameState {
    NotStarted,
    InProgress,
    Won,
    Lost
};

struct RevealedTile {
    Position pos;
    int adjacentMines;
    bool isMine;
};

struct RevealResult {
    std::vector<RevealedTile> revealed;
    GameState newState;
};

class Game {
public:
    explicit Game(const GameConfig& config);

    RevealResult reveal(const Position& pos);
    void flag(const Position& pos);
    void reset();

    GameState getState() const;
    const Board& getBoard() const;
    const GameConfig& getConfig() const;

private:
    GameConfig config;
    Board board;
    GameState state;

    void checkWinCondition();
};
