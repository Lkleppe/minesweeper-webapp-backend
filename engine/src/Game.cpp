#include "Game.h"

Game::Game(const GameConfig& config)
    : config(config), board(config.rows, config.cols), state(GameState::NotStarted) {}

RevealResult Game::reveal(const Position& pos) {
    RevealResult result;

    if (state == GameState::Won || state == GameState::Lost) {
        result.newState = state;
        return result;
    }

    if (state == GameState::NotStarted) {
        board.placeMines(config.mineCount, pos);
        board.calculateAdjacentMines();
        state = GameState::InProgress;
    }

    if (board.getTile(pos).getState() != TileState::Hidden) {
        result.newState = state;
        return result;
    }

    std::vector<Position> revealedPositions = board.revealTile(pos);

    for (const auto& p : revealedPositions) {
        const Tile& tile = board.getTile(p);
        result.revealed.push_back({p, tile.getAdjacentMines(), tile.isMine()});
    }

    if (board.getTile(pos).isMine()) {
        state = GameState::Lost;
    } else {
        checkWinCondition();
    }

    result.newState = state;
    return result;
}

void Game::flag(const Position& pos) {
    if (state != GameState::InProgress) return;
    board.flagTile(pos);
}

void Game::reset() {
    board = Board(config.rows, config.cols);
    state = GameState::NotStarted;
}

GameState Game::getState() const { return state; }
const Board& Game::getBoard() const { return board; }
const GameConfig& Game::getConfig() const { return config; }

void Game::checkWinCondition() {
    int nonMineTiles = config.rows * config.cols - config.mineCount;
    if (board.getRevealedCount() == nonMineTiles)
        state = GameState::Won;
}
