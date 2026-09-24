#include "BoardTest.h"
#include "Board.h"

CPPUNIT_TEST_SUITE_REGISTRATION(BoardTest);

void BoardTest::testDimensions() {
    Board board(5, 8);
    CPPUNIT_ASSERT_EQUAL(5, board.getRows());
    CPPUNIT_ASSERT_EQUAL(8, board.getCols());
}

void BoardTest::testIsInBounds() {
    Board board(4, 4);
    CPPUNIT_ASSERT(board.isInBounds({0, 0}));
    CPPUNIT_ASSERT(board.isInBounds({3, 3}));
    CPPUNIT_ASSERT(!board.isInBounds({-1, 0}));
    CPPUNIT_ASSERT(!board.isInBounds({0, -1}));
    CPPUNIT_ASSERT(!board.isInBounds({4, 0}));
    CPPUNIT_ASSERT(!board.isInBounds({0, 4}));
}

void BoardTest::testGetAdjacentPositions_corner() {
    Board board(5, 5);
    auto neighbors = board.getAdjacentPositions({0, 0});
    CPPUNIT_ASSERT_EQUAL(3, (int)neighbors.size());
}

void BoardTest::testGetAdjacentPositions_center() {
    Board board(5, 5);
    auto neighbors = board.getAdjacentPositions({2, 2});
    CPPUNIT_ASSERT_EQUAL(8, (int)neighbors.size());
}

void BoardTest::testPlaceMines_correctCount() {
    Board board(5, 5);
    board.placeMines(10, {2, 2});
    int count = 0;
    for (int r = 0; r < 5; ++r)
        for (int c = 0; c < 5; ++c)
            if (board.getTile({r, c}).isMine()) ++count;
    CPPUNIT_ASSERT_EQUAL(10, count);
    CPPUNIT_ASSERT_EQUAL(10, board.getMineCount());
}

void BoardTest::testPlaceMines_safePosNotMine() {
    Board board(9, 9);
    Position safePos{4, 4};
    for (int i = 0; i < 20; ++i) {
        Board b(9, 9);
        b.placeMines(10, safePos);
        CPPUNIT_ASSERT(!b.getTile(safePos).isMine());
    }
}

void BoardTest::testCalculateAdjacentMines() {
    Board board(3, 3);
    board.getTile({0, 0}).setMine(true);
    board.calculateAdjacentMines();

    CPPUNIT_ASSERT_EQUAL(1, board.getTile({0, 1}).getAdjacentMines());
    CPPUNIT_ASSERT_EQUAL(1, board.getTile({1, 0}).getAdjacentMines());
    CPPUNIT_ASSERT_EQUAL(1, board.getTile({1, 1}).getAdjacentMines());
    CPPUNIT_ASSERT_EQUAL(0, board.getTile({0, 2}).getAdjacentMines());
    CPPUNIT_ASSERT_EQUAL(0, board.getTile({2, 2}).getAdjacentMines());
}

void BoardTest::testRevealTile_singleTile() {
    Board board(3, 3);
    board.getTile({0, 0}).setMine(true);
    board.calculateAdjacentMines();

    // {0,1} is adjacent to the mine at {0,0}, so adjacentMines > 0 — no cascade
    auto revealed = board.revealTile({0, 1});
    CPPUNIT_ASSERT_EQUAL(TileState::Revealed, board.getTile({0, 1}).getState());
    CPPUNIT_ASSERT_EQUAL(1, (int)revealed.size());
    Position expected{0, 1};
    CPPUNIT_ASSERT(revealed[0] == expected);
}

void BoardTest::testRevealTile_floodFill() {
    Board board(3, 3);
    // No mines — every tile has 0 adjacent mines, so the whole board cascades
    board.placeMines(0, {1, 1});
    board.calculateAdjacentMines();

    auto revealed = board.revealTile({1, 1});
    CPPUNIT_ASSERT_EQUAL(9, (int)revealed.size());
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            CPPUNIT_ASSERT_EQUAL(TileState::Revealed, board.getTile({r, c}).getState());
}

void BoardTest::testRevealTile_stopsAtNumberedTiles() {
    // Place a mine at (0,0). Tiles adjacent to it get adjacentMines > 0.
    // Revealing (2,2) — far corner, 0 adjacent mines — should cascade but stop
    // at the numbered border tiles and not reveal the mine itself.
    Board board(5, 5);
    board.getTile({0, 0}).setMine(true);
    board.calculateAdjacentMines();

    auto revealed = board.revealTile({4, 4});

    // The mine must not be revealed
    CPPUNIT_ASSERT(board.getTile({0, 0}).getState() != TileState::Revealed);
    // At least the clicked tile was revealed
    CPPUNIT_ASSERT(board.getTile({4, 4}).getState() == TileState::Revealed);
    // More than one tile was revealed (cascade happened)
    CPPUNIT_ASSERT((int)revealed.size() > 1);
}

void BoardTest::testRevealTile_noopOnAlreadyRevealed() {
    Board board(3, 3);
    board.placeMines(0, {1, 1});
    board.calculateAdjacentMines();

    board.revealTile({1, 1});
    auto second = board.revealTile({1, 1});
    CPPUNIT_ASSERT_EQUAL(0, (int)second.size());
}

void BoardTest::testFlagTile_toggle() {
    Board board(3, 3);
    board.flagTile({1, 1});
    CPPUNIT_ASSERT_EQUAL(TileState::Flagged, board.getTile({1, 1}).getState());
    board.flagTile({1, 1});
    CPPUNIT_ASSERT_EQUAL(TileState::Hidden, board.getTile({1, 1}).getState());
}

void BoardTest::testGetRevealedCount() {
    Board board(3, 3);
    board.placeMines(0, {0, 0});
    board.calculateAdjacentMines();

    CPPUNIT_ASSERT_EQUAL(0, board.getRevealedCount());
    board.revealTile({1, 1});
    CPPUNIT_ASSERT_EQUAL(9, board.getRevealedCount());
}
