cmake_minimum_required(VERSION 3.15)

find_path(CPPUNIT_INCLUDE_DIR cppunit/TestCase.h
    HINTS /opt/homebrew/include /usr/local/include
)

find_library(CPPUNIT_LIBRARY cppunit
    HINTS /opt/homebrew/lib /usr/local/lib
)

if(NOT CPPUNIT_INCLUDE_DIR OR NOT CPPUNIT_LIBRARY)
    message(FATAL_ERROR "CppUnit not found. Install with: brew install cppunit")
endif()

add_executable(engine_tests
    main.cpp
    TileTest.cpp
    BoardTest.cpp
    GameTest.cpp
)

target_link_libraries(engine_tests
    PRIVATE
    engine_lib
    ${CPPUNIT_LIBRARY}
)

target_include_directories(engine_tests
    PRIVATE
    ${CPPUNIT_INCLUDE_DIR}
)
