# Minesweeper Engine

This directory contains the core game logic for the minesweeper engine. The engine is completely self-contained — it knows nothing about networking, serialization, or the web layer. The server creates one `Game` instance per active game session, sends player commands to it, and reads back a result describing what changed.

---

## Architecture Overview

The engine is organized into five classes across five headers:

```
Position        — a (row, col) coordinate pair
Tile            — a single cell on the board
Board           — the 2D grid of Tiles; owns mine placement and reveal logic
GameConfig      — configuration (dimensions + mine count)
Game            — top-level entry point; the only class the server needs to touch
```

The server exclusively talks to `Game`. `Board` and `Tile` are implementation details. `Position` and `GameConfig` are plain data types used in the public interface.

---

## Class Reference

### `Position`

```cpp
struct Position {
    int row;
    int col;
};
```

A zero-indexed (row, col) coordinate. Row 0 is the top row; col 0 is the leftmost column. Supports `==` and `!=`.

---

### `TileState` and `Tile`

Each cell on the board is a `Tile`. A tile has three possible states, represented by `TileState`:

| State      | Meaning                                              |
|------------|------------------------------------------------------|
| `Hidden`   | Not yet interacted with. Default state.              |
| `Revealed` | The player clicked it; its contents are now visible. |
| `Flagged`  | The player marked it as a suspected mine.            |

**Key behaviors:**
- `flag()` only works on a `Hidden` tile. Calling it on a `Revealed` tile is a no-op.
- `unflag()` only works on a `Flagged` tile. Calling it on a `Hidden` tile is a no-op.
- `reveal()` unconditionally sets the state to `Revealed`. The caller is responsible for ensuring this is only called when appropriate.
- A tile knows whether it is a mine (`isMine()`) and how many of its up-to-8 neighbors are mines (`getAdjacentMines()`). Both values are 0/false until `Board::placeMines` and `Board::calculateAdjacentMines` are called.

---

### `Difficulty` and `GameConfig`

`GameConfig` holds the three numbers that define a game: board dimensions and mine count.

```cpp
struct GameConfig {
    int rows;
    int cols;
    int mineCount;
    Difficulty difficulty;  // Beginner | Intermediate | Expert | Custom
};
```

**Preset factory methods:**

| Method                             | Rows | Cols | Mines |
|------------------------------------|------|------|-------|
| `GameConfig::beginner()`           | 9    | 9    | 10    |
| `GameConfig::intermediate()`       | 16   | 16   | 40    |
| `GameConfig::expert()`             | 16   | 30   | 99    |
| `GameConfig::custom(r, c, mines)`  | r    | c    | mines |

---

### `Board`

`Board` is the internal data structure. The server does not need to call any of its methods directly — `Game` does all of that — but the server may read board state via `game.getBoard()` after an action if it needs to inspect individual tiles.

**Construction:**

```cpp
Board(int rows, int cols);
```

Creates a blank grid of `rows × cols` hidden, non-mine tiles. Mine count starts at 0; no mines are placed until `placeMines` is called.

**Mine placement (called by `Game::reveal` on the first click):**

```cpp
void placeMines(int mineCount, const Position& safePos);
```

Randomly distributes `mineCount` mines across the board using `std::mt19937` seeded from `std::random_device`. The tile at `safePos` is excluded from consideration, guaranteeing the player's first click is never a mine.

After placing mines, the board calls:

```cpp
void calculateAdjacentMines();
```

This iterates every non-mine tile and counts how many of its up-to-8 neighbors are mines, storing the result in each `Tile`. This is what produces the numbers the player sees.

**Reveal and BFS flood fill:**

```cpp
std::vector<Position> revealTile(const Position& pos);
```

Reveals the tile at `pos` and returns a list of every tile that was newly revealed as a result. The behavior depends on the tile:

- If the tile is not `Hidden`, nothing happens and an empty list is returned.
- If the tile is a mine or has `adjacentMines > 0`, only that single tile is revealed.
- If the tile has `adjacentMines == 0`, a **breadth-first search** propagates outward:
  - Every hidden neighbor is revealed and added to the result.
  - Neighbors with `adjacentMines == 0` are enqueued and also propagate.
  - Neighbors with `adjacentMines > 0` are revealed but not enqueued — they act as the visible border of the opened region, and the cascade stops there.
  - Mine tiles are never enqueued (a 0-adjacent-mine tile by definition has no mine neighbors, so the BFS never reaches a mine through cascade).

**Flag toggling:**

```cpp
void flagTile(const Position& pos);
```

Toggles the flag state of a tile:
- `Hidden` → `Flagged`
- `Flagged` → `Hidden`
- `Revealed` → no-op

**Read-only accessors:**

```cpp
Tile& getTile(const Position& pos);          // mutable
const Tile& getTile(const Position& pos);    // read-only
bool isInBounds(const Position& pos) const;
std::vector<Position> getAdjacentPositions(const Position& pos) const;
int getRows() const;
int getCols() const;
int getMineCount() const;
int getRevealedCount() const;
```

`getAdjacentPositions` returns between 3 and 8 positions depending on whether `pos` is a corner, edge, or interior tile. Out-of-bounds neighbors are automatically excluded.

---

### `GameState`

```cpp
enum class GameState {
    NotStarted,   // No moves made yet. Mines have not been placed.
    InProgress,   // Game is active.
    Won,          // All non-mine tiles have been revealed.
    Lost          // A mine tile was revealed.
};
```

---

### `RevealedTile` and `RevealResult`

These are the return types of `Game::reveal` — the message the server reads to find out what happened.

```cpp
struct RevealedTile {
    Position pos;         // which tile was revealed
    int adjacentMines;    // number of neighboring mines (0–8)
    bool isMine;          // true if this tile was a mine
};

struct RevealResult {
    std::vector<RevealedTile> revealed;   // all tiles newly revealed by this action
    GameState newState;                   // game state after the action
};
```

`revealed` contains every tile that changed from `Hidden` to `Revealed` as a result of this single call — including all tiles opened by BFS cascade. The server can send this list directly to the client to update the UI without re-querying the full board.

If the action was a no-op (tile already revealed, or game is over), `revealed` is empty and `newState` reflects the current state.

---

### `Game`

`Game` is the only class the server needs to instantiate and call. It owns a `Board` and `GameConfig` internally.

**Construction:**

```cpp
explicit Game(const GameConfig& config);
```

Creates a new game with a blank board. State is `NotStarted`. No mines are placed yet.

---

## Interfacing With the Engine

### Starting a game

```cpp
Game game(GameConfig::beginner());
// or
Game game(GameConfig::custom(20, 20, 60));
```

### Sending a reveal command

```cpp
RevealResult result = game.reveal({row, col});
```

On the **first call**, the engine places mines (excluding the clicked tile), calculates adjacent mine counts, and transitions to `InProgress` — all before revealing the tile. This guarantees the first click is always safe.

On subsequent calls:
- If the tile is a mine: the tile is revealed, `newState` is `Lost`, and `result.revealed` contains that one mine tile with `isMine = true`.
- If the tile has adjacent mines: just that tile is revealed.
- If the tile has zero adjacent mines: BFS cascade opens a region; `result.revealed` contains every tile that was uncovered.
- If the tile is not `Hidden` (already revealed or flagged): no-op, `result.revealed` is empty.
- If the game is already `Won` or `Lost`: no-op, `result.revealed` is empty.

After every reveal, check `result.newState` to determine if the game ended.

### Sending a flag command

```cpp
game.flag({row, col});
```

Toggles the flag on the tile. Only works when `GameState` is `InProgress`. No-op before the first click or after the game ends. No return value — read back the tile state from `game.getBoard().getTile(pos).getState()` if needed.

### Resetting a game

```cpp
game.reset();
```

Replaces the board with a fresh blank grid and sets state back to `NotStarted`. The `GameConfig` is preserved, so the next game has the same dimensions and mine count.

### Reading board state

```cpp
const Board& board = game.getBoard();
const Tile& tile = board.getTile({r, c});

tile.getState();        // Hidden | Revealed | Flagged
tile.isMine();          // true if mine (only meaningful after first reveal)
tile.getAdjacentMines(); // 0–8 (only meaningful after first reveal)
```

---

## Full Example

```cpp
#include "Game.h"

// Create a beginner game
Game game(GameConfig::beginner());

// Player clicks tile (4, 4) — first click, always safe
RevealResult result = game.reveal({4, 4});

// result.newState == GameState::InProgress
// result.revealed contains every tile uncovered by the BFS cascade

for (const RevealedTile& t : result.revealed) {
    // send t.pos, t.adjacentMines to the client
}

// Player flags a tile
game.flag({0, 0});

// Player clicks another tile
RevealResult result2 = game.reveal({0, 1});

if (result2.newState == GameState::Lost) {
    // result2.revealed[0].isMine == true
    // game over — notify client
} else if (result2.newState == GameState::Won) {
    // all non-mine tiles revealed — player wins
}

// Start a new round with the same config
game.reset();
```

---

## Game State Transitions

```
NotStarted ──(first reveal)──► InProgress ──(mine revealed)──► Lost
                                    │
                                    └──(all non-mine tiles revealed)──► Won
```

- `Won` and `Lost` are terminal. No further reveals or flags have any effect.
- `reset()` returns to `NotStarted` from any state.
- Flagging is only allowed in `InProgress`.
