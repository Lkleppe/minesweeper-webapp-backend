#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TileTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TileTest);
    CPPUNIT_TEST(testDefaultState);
    CPPUNIT_TEST(testReveal);
    CPPUNIT_TEST(testFlag);
    CPPUNIT_TEST(testUnflag);
    CPPUNIT_TEST(testFlagOnRevealedIsNoop);
    CPPUNIT_TEST(testUnflagOnHiddenIsNoop);
    CPPUNIT_TEST(testSetMine);
    CPPUNIT_TEST(testSetAdjacentMines);
    CPPUNIT_TEST_SUITE_END();

public:
    void testDefaultState();
    void testReveal();
    void testFlag();
    void testUnflag();
    void testFlagOnRevealedIsNoop();
    void testUnflagOnHiddenIsNoop();
    void testSetMine();
    void testSetAdjacentMines();
};
