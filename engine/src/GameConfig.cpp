#include "GameConfig.h"

GameConfig GameConfig::beginner() {
    return {9, 9, 10, Difficulty::Beginner};
}

GameConfig GameConfig::intermediate() {
    return {16, 16, 40, Difficulty::Intermediate};
}

GameConfig GameConfig::expert() {
    return {30, 16, 99, Difficulty::Expert};
}

GameConfig GameConfig::custom(int rows, int cols, int mineCount) {
    return {rows, cols, mineCount, Difficulty::Custom};
}
