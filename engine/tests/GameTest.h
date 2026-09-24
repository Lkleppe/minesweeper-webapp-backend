#pragma once

#include <cppunit/extensions/HelperMacros.h>

class GameTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(GameTest);
    CPPUNIT_TEST(testInitialState);
    CPPUNIT_TEST(testFirstRevealStartsGame);
    CPPUNIT_TEST(testFirstRevealIsNeverMine);
    CPPUNIT_TEST(testRevealMine_gameLost);
    CPPUNIT_TEST(testRevealResult_containsClickedTile);
    CPPUNIT_TEST(testRevealResult_floodFillReturnsMultipleTiles);
    CPPUNIT_TEST(testWinCondition);
    CPPUNIT_TEST(testNoActionsAfterGameOver);
    CPPUNIT_TEST(testFlag_noopWhenNotStarted);
    CPPUNIT_TEST(testFlag_togglesOnBoard);
    CPPUNIT_TEST(testReset);
    CPPUNIT_TEST_SUITE_END();

public:
    void testInitialState();
    void testFirstRevealStartsGame();
    void testFirstRevealIsNeverMine();
    void testRevealMine_gameLost();
    void testRevealResult_containsClickedTile();
    void testRevealResult_floodFillReturnsMultipleTiles();
    void testWinCondition();
    void testNoActionsAfterGameOver();
    void testFlag_noopWhenNotStarted();
    void testFlag_togglesOnBoard();
    void testReset();
};
