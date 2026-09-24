#pragma once

enum class Difficulty {
    Beginner,
    Intermediate,
    Expert,
    Custom
};

struct GameConfig {
    int rows;
    int cols;
    int mineCount;
    Difficulty difficulty;

    static GameConfig beginner();
    static GameConfig intermediate();
    static GameConfig expert();
    static GameConfig custom(int rows, int cols, int mineCount);
};
