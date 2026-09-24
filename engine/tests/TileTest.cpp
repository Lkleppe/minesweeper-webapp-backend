#include "TileTest.h"
#include "Tile.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TileTest);

void TileTest::testDefaultState() {
    Tile tile;
    CPPUNIT_ASSERT_EQUAL(TileState::Hidden, tile.getState());
    CPPUNIT_ASSERT_EQUAL(false, tile.isMine());
    CPPUNIT_ASSERT_EQUAL(0, tile.getAdjacentMines());
}

void TileTest::testReveal() {
    Tile tile;
    tile.reveal();
    CPPUNIT_ASSERT_EQUAL(TileState::Revealed, tile.getState());
}

void TileTest::testFlag() {
    Tile tile;
    tile.flag();
    CPPUNIT_ASSERT_EQUAL(TileState::Flagged, tile.getState());
}

void TileTest::testUnflag() {
    Tile tile;
    tile.flag();
    tile.unflag();
    CPPUNIT_ASSERT_EQUAL(TileState::Hidden, tile.getState());
}

void TileTest::testFlagOnRevealedIsNoop() {
    Tile tile;
    tile.reveal();
    tile.flag();
    CPPUNIT_ASSERT_EQUAL(TileState::Revealed, tile.getState());
}

void TileTest::testUnflagOnHiddenIsNoop() {
    Tile tile;
    tile.unflag();
    CPPUNIT_ASSERT_EQUAL(TileState::Hidden, tile.getState());
}

void TileTest::testSetMine() {
    Tile tile;
    tile.setMine(true);
    CPPUNIT_ASSERT_EQUAL(true, tile.isMine());
    tile.setMine(false);
    CPPUNIT_ASSERT_EQUAL(false, tile.isMine());
}

void TileTest::testSetAdjacentMines() {
    Tile tile;
    tile.setAdjacentMines(5);
    CPPUNIT_ASSERT_EQUAL(5, tile.getAdjacentMines());
}
