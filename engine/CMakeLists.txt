cmake_minimum_required(VERSION 3.15)

add_library(engine_lib
    src/Tile.cpp
    src/Position.cpp
    src/GameConfig.cpp
    src/Board.cpp
    src/Game.cpp
)

target_include_directories(engine_lib PUBLIC src)

add_subdirectory(tests)
