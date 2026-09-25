# Engine Tests

32 CppUnit tests across three test suites: `TileTest`, `BoardTest`, and `GameTest`. Each suite is registered with CppUnit's global factory and run together by `main.cpp`.

## Running the Tests

From the repo root (`minesweeper-webapp-backend/`):

```bash
cmake -S . -B build && cmake --build build
./build/engine/tests/engine_tests
```

Output is printed per-test by `BriefTestProgressListener`. Failures are printed to stderr in compiler-error format at the end.

---

## TileTest — 8 tests

Tests the `Tile` class in isolation. No `Board` or `Game` involvement. Each test constructs a fresh `Tile` from scratch.

---

### `testDefaultState`

**What it checks:** A newly constructed `Tile` has the correct zero state.

**Setup:** `Tile tile;`

**Assertions:**
- `getState()` == `TileState::Hidden` — tile starts hidden, not yet interacted with
- `isMine()` == `false` — tile is not a mine until explicitly set
- `getAdjacentMines()` == `0` — no adjacent mine count assigned yet

---

### `testReveal`

**What it checks:** Calling `reveal()` on a hidden tile transitions it to `Revealed`.

**Setup:** Default tile, then `tile.reveal()`.

**Assertions:**
- `getState()` == `TileState::Revealed`

---

### `testFlag`

**What it checks:** Calling `flag()` on a hidden tile transitions it to `Flagged`.

**Setup:** Default tile, then `tile.flag()`.

**Assertions:**
- `getState()` == `TileState::Flagged`

---

### `testUnflag`

**What it checks:** Calling `unflag()` on a flagged tile returns it to `Hidden`.

**Setup:** Default tile → `flag()` → `unflag()`.

**Assertions:**
- `getState()` == `TileState::Hidden`

---

### `testFlagOnRevealedIsNoop`

**What it checks:** `flag()` has no effect on an already-revealed tile. A player cannot flag something they've already uncovered.

**Setup:** Default tile → `reveal()` → `flag()`.

**Assertions:**
- `getState()` == `TileState::Revealed` — state did not change

---

### `testUnflagOnHiddenIsNoop`

**What it checks:** `unflag()` has no effect on a hidden tile (i.e. you cannot unflag something that was never flagged).

**Setup:** Default tile → `unflag()`.

**Assertions:**
- `getState()` == `TileState::Hidden` — state did not change

---

### `testSetMine`

**What it checks:** `setMine()` correctly sets and clears the mine flag.

**Setup:** Default tile.

**Assertions:**
- After `setMine(true)`: `isMine()` == `true`
- After `setMine(false)`: `isMine()` == `false`

---

### `testSetAdjacentMines`

**What it checks:** `setAdjacentMines()` stores the value and `getAdjacentMines()` retrieves it.

**Setup:** Default tile → `setAdjacentMines(5)`.

**Assertions:**
- `getAdjacentMines()` == `5`

---

## BoardTest — 13 tests

Tests `Board` directly, bypassing `Game`. Several tests manually call `setMine()` on individual tiles to set up deterministic board states without relying on random mine placement.

---

### `testDimensions`

**What it checks:** `getRows()` and `getCols()` return the values passed to the constructor.

**Setup:** `Board board(5, 8)`

**Assertions:**
- `getRows()` == `5`
- `getCols()` == `8`

---

### `testIsInBounds`

**What it checks:** `isInBounds()` correctly accepts valid positions and rejects out-of-bounds ones, including negative indices.

**Setup:** `Board board(4, 4)`

**Assertions (true):**
- `{0, 0}` — top-left corner
- `{3, 3}` — bottom-right corner (last valid index on a 4×4 board)

**Assertions (false):**
- `{-1, 0}` — negative row
- `{0, -1}` — negative col
- `{4, 0}` — row equals board height (off by one)
- `{0, 4}` — col equals board width (off by one)

---

### `testGetAdjacentPositions_corner`

**What it checks:** A corner tile has exactly 3 neighbors, not 8, because the remaining 5 directions are out of bounds.

**Setup:** `Board board(5, 5)`, query `{0, 0}`.

**Assertions:**
- `getAdjacentPositions({0, 0}).size()` == `3`

---

### `testGetAdjacentPositions_center`

**What it checks:** A fully interior tile has exactly 8 neighbors — all 8 directions are in bounds.

**Setup:** `Board board(5, 5)`, query `{2, 2}`.

**Assertions:**
- `getAdjacentPositions({2, 2}).size()` == `8`

---

### `testPlaceMines_correctCount`

**What it checks:** After calling `placeMines(10, safePos)`, exactly 10 tiles on the board have `isMine() == true`, and `getMineCount()` also returns 10.

**Setup:** `Board board(5, 5)`, `placeMines(10, {2, 2})`. Iterates all 25 tiles and counts mines manually.

**Assertions:**
- Manual count == `10`
- `getMineCount()` == `10`

---

### `testPlaceMines_safePosNotMine`

**What it checks:** The `safePos` passed to `placeMines` is never assigned a mine, across repeated random placements.

**Setup:** Runs 20 independent placements on a 9×9 board with 10 mines, each time passing `{4, 4}` as the safe position.

**Assertions (per iteration):**
- `getTile({4, 4}).isMine()` == `false`

**Why 20 iterations:** Mine placement uses a seeded RNG. Without looping, a single run could pass by luck even if the safe-pos exclusion logic were broken. 20 iterations makes an accidental pass statistically negligible.

---

### `testCalculateAdjacentMines`

**What it checks:** After placing a mine manually at `{0, 0}` and calling `calculateAdjacentMines()`, all tiles adjacent to that mine have their `adjacentMines` count incremented to 1, and tiles with no mine neighbors remain at 0.

**Setup:** 3×3 board, `getTile({0, 0}).setMine(true)`, then `calculateAdjacentMines()`.

**Assertions:**
- `{0, 1}.getAdjacentMines()` == `1` — shares a row with the mine
- `{1, 0}.getAdjacentMines()` == `1` — shares a column with the mine
- `{1, 1}.getAdjacentMines()` == `1` — diagonally adjacent
- `{0, 2}.getAdjacentMines()` == `0` — not adjacent to any mine
- `{2, 2}.getAdjacentMines()` == `0` — far corner, no mine nearby

---

### `testRevealTile_singleTile`

**What it checks:** Revealing a tile that has `adjacentMines > 0` reveals exactly that one tile and does not cascade.

**Setup:** 3×3 board, mine at `{0, 0}`. After `calculateAdjacentMines()`, tile `{0, 1}` has `adjacentMines == 1`. `revealTile({0, 1})` is called.

**Why `{0, 1}` and not e.g. `{2, 2}`:** `{2, 2}` is the opposite corner from the mine and has `adjacentMines == 0`, which would trigger a cascade and reveal more than one tile. `{0, 1}` is directly adjacent to the mine so it has a non-zero count and will not propagate.

**Assertions:**
- `getTile({0, 1}).getState()` == `TileState::Revealed`
- `revealed.size()` == `1`
- `revealed[0]` == `{0, 1}`

---

### `testRevealTile_floodFill`

**What it checks:** Revealing a tile with `adjacentMines == 0` triggers a BFS cascade that reveals the entire reachable region. With zero mines on the board, every tile has `adjacentMines == 0`, so the entire board cascades from a single click.

**Setup:** 3×3 board, `placeMines(0, {1, 1})`, `calculateAdjacentMines()`. All tiles have `adjacentMines == 0`.

**Assertions:**
- `revealed.size()` == `9` — all 9 tiles were revealed in one call
- Every tile on the board has `getState() == TileState::Revealed`

---

### `testRevealTile_stopsAtNumberedTiles`

**What it checks:** The BFS cascade opens the zero-adjacent-mine region and reveals the numbered tiles bordering it, but does not continue beyond them — and specifically does not reveal mine tiles.

**Setup:** 5×5 board, mine manually placed at `{0, 0}`. The tiles adjacent to `{0, 0}` get `adjacentMines > 0` after `calculateAdjacentMines()`. Tile `{4, 4}` is far from the mine and has `adjacentMines == 0`. `revealTile({4, 4})` is called.

**Assertions:**
- `getTile({0, 0}).getState()` != `TileState::Revealed` — the mine was not revealed
- `getTile({4, 4}).getState()` == `TileState::Revealed` — the clicked tile was revealed
- `revealed.size()` > `1` — cascade did happen (more than just the clicked tile)

---

### `testRevealTile_noopOnAlreadyRevealed`

**What it checks:** Calling `revealTile` on a tile that is already revealed returns an empty list and does not double-count or re-add it.

**Setup:** 3×3 board with no mines. `revealTile({1, 1})` is called once (reveals the full board). Then `revealTile({1, 1})` is called a second time.

**Assertions:**
- Second call returns a vector of size `0`

---

### `testFlagTile_toggle`

**What it checks:** `flagTile` toggles a tile from `Hidden` to `Flagged` on the first call and back to `Hidden` on the second call.

**Setup:** 3×3 board (no mines needed). `flagTile({1, 1})` called twice.

**Assertions:**
- After first call: `getTile({1, 1}).getState()` == `TileState::Flagged`
- After second call: `getTile({1, 1}).getState()` == `TileState::Hidden`

---

### `testGetRevealedCount`

**What it checks:** `getRevealedCount()` returns 0 on a fresh board and correctly counts all revealed tiles after a cascade.

**Setup:** 3×3 board, no mines, `placeMines(0, {0, 0})`, `calculateAdjacentMines()`. All tiles have `adjacentMines == 0`.

**Assertions:**
- Before any reveal: `getRevealedCount()` == `0`
- After `revealTile({1, 1})` (cascades entire board): `getRevealedCount()` == `9`

---

## GameTest — 11 tests

Tests the `Game` class through its public interface only — no direct `Board` or `Tile` manipulation except to read state via `game.getBoard()` after an action. These tests exercise the full game lifecycle.

---

### `testInitialState`

**What it checks:** A newly constructed `Game` is in `NotStarted` state before any player action.

**Setup:** `Game game(GameConfig::beginner())`

**Assertions:**
- `game.getState()` == `GameState::NotStarted`

---

### `testFirstRevealStartsGame`

**What it checks:** The first call to `reveal()` transitions the game from `NotStarted` to `InProgress`. This is when mine placement and adjacency calculation happen internally.

**Setup:** `Game game(GameConfig::beginner())`, then `game.reveal({4, 4})`.

**Assertions:**
- `game.getState()` == `GameState::InProgress`

---

### `testFirstRevealIsNeverMine`

**What it checks:** The clicked tile is always excluded from mine placement, so the first reveal can never be a mine and can never result in `Lost`.

**Setup:** Runs 50 independent beginner games, each revealing `{4, 4}` as the first move.

**Assertions (per iteration):**
- `result.revealed` is not empty
- `result.revealed[0].isMine` == `false` — the clicked tile is not a mine
- `game.getState()` != `GameState::Lost`

**Why 50 iterations:** Mine placement is random. A single run could pass even if the safe-pos exclusion were broken. 50 iterations makes an accidental pass negligible.

---

### `testRevealMine_gameLost`

**What it checks:** Revealing a mine tile sets `newState` to `Lost`.

**Setup:** 10×10 board with 90 mines. After the first safe click at `{0, 0}`, the board is scanned for any mine tile and that tile is revealed.

**Why 10×10/90 mines:** With 90 mines in 100 tiles, every non-mine tile is surrounded by mines and has `adjacentMines > 0`. This prevents any cascade on the first click, ensuring the game is `InProgress` (not immediately `Won`) after the first reveal. The high mine density also guarantees a mine is trivially findable for the second click.

**Assertions:**
- After first click: `game.getState()` == `GameState::InProgress`
- After clicking a mine: `result.newState` == `GameState::Lost`

---

### `testRevealResult_containsClickedTile`

**What it checks:** The `RevealResult` returned by `reveal()` always contains the tile that was explicitly clicked, regardless of whether a cascade occurred.

**Setup:** 5×5 board with 1 mine. `game.reveal({2, 2})`.

**Assertions:**
- `result.revealed` is not empty
- At least one entry in `result.revealed` has `pos == {2, 2}`

---

### `testRevealResult_floodFillReturnsMultipleTiles`

**What it checks:** When a cascade occurs, `RevealResult.revealed` contains more than one tile — confirming that the BFS result is correctly propagated from `Board` through `Game` into the return value.

**Setup:** 9×9 board with 1 mine. First click at `{4, 4}` (center). With only 1 mine on an 81-tile board, the center tile almost certainly has `adjacentMines == 0` and a large cascade is expected.

**Assertions:**
- `result.revealed.size()` > `1`

---

### `testWinCondition`

**What it checks:** Revealing every non-mine tile transitions the game to `Won`.

**Setup:** 3×3 board with 1 mine. After the first click at `{1, 1}`, the board is iterated and every tile where `isMine() == false` is revealed via `game.reveal()`.

**Why this works deterministically:** After the first reveal, mines are placed and `isMine()` is readable through `game.getBoard()`. The test can therefore skip the mine and reveal everything else without guessing.

**Assertions:**
- `game.getState()` == `GameState::Won`

---

### `testNoActionsAfterGameOver`

**What it checks:** Once the game reaches `Lost`, further calls to `reveal()` are no-ops — they return an empty `revealed` list and do not change the state.

**Setup:** 10×10/90-mine board. First click at `{0, 0}` starts the game. A mine tile is found and revealed, setting state to `Lost`. Then `reveal({0, 0})` is called again.

**Assertions:**
- `game.getState()` == `GameState::Lost` after hitting the mine
- Second reveal: `result.revealed.empty()` == `true`
- Second reveal: `result.newState` == `GameState::Lost`

---

### `testFlag_noopWhenNotStarted`

**What it checks:** Calling `flag()` before the first reveal (while state is `NotStarted`) has no effect on the board.

**Setup:** Fresh beginner game. `game.flag({0, 0})` without any prior `reveal()`.

**Assertions:**
- `getTile({0, 0}).getState()` == `TileState::Hidden` — the tile was not flagged

---

### `testFlag_togglesOnBoard`

**What it checks:** During an active game, `flag()` toggles a tile from `Hidden` to `Flagged` and back to `Hidden` on a second call. Verifies that flagging is correctly delegated to the board.

**Setup:** Beginner game, first click at `{4, 4}` to start. Then `game.flag({0, 0})` twice.

**Assertions:**
- After first flag: `getTile({0, 0}).getState()` == `TileState::Flagged`
- After second flag: `getTile({0, 0}).getState()` == `TileState::Hidden`

---

### `testReset`

**What it checks:** `reset()` returns the game to `NotStarted` and clears all revealed tiles, while preserving the original `GameConfig`.

**Setup:** Beginner game, first click at `{4, 4}` to enter `InProgress`. Then `game.reset()`.

**Assertions:**
- Before reset: `game.getState()` == `GameState::InProgress`
- After reset: `game.getState()` == `GameState::NotStarted`
- After reset: `game.getBoard().getRevealedCount()` == `0`
