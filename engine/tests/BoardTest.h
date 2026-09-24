#pragma once

#include <cppunit/extensions/HelperMacros.h>

class BoardTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(BoardTest);
    CPPUNIT_TEST(testDimensions);
    CPPUNIT_TEST(testIsInBounds);
    CPPUNIT_TEST(testGetAdjacentPositions_corner);
    CPPUNIT_TEST(testGetAdjacentPositions_center);
    CPPUNIT_TEST(testPlaceMines_correctCount);
    CPPUNIT_TEST(testPlaceMines_safePosNotMine);
    CPPUNIT_TEST(testCalculateAdjacentMines);
    CPPUNIT_TEST(testRevealTile_singleTile);
    CPPUNIT_TEST(testRevealTile_floodFill);
    CPPUNIT_TEST(testRevealTile_stopsAtNumberedTiles);
    CPPUNIT_TEST(testRevealTile_noopOnAlreadyRevealed);
    CPPUNIT_TEST(testFlagTile_toggle);
    CPPUNIT_TEST(testGetRevealedCount);
    CPPUNIT_TEST_SUITE_END();

public:
    void testDimensions();
    void testIsInBounds();
    void testGetAdjacentPositions_corner();
    void testGetAdjacentPositions_center();
    void testPlaceMines_correctCount();
    void testPlaceMines_safePosNotMine();
    void testCalculateAdjacentMines();
    void testRevealTile_singleTile();
    void testRevealTile_floodFill();
    void testRevealTile_stopsAtNumberedTiles();
    void testRevealTile_noopOnAlreadyRevealed();
    void testFlagTile_toggle();
    void testGetRevealedCount();
};
