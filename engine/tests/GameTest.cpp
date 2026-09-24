#include "GameTest.h"
#include "Game.h"

CPPUNIT_TEST_SUITE_REGISTRATION(GameTest);

void GameTest::testInitialState() {
    Game game(GameConfig::beginner());
    CPPUNIT_ASSERT_EQUAL(GameState::NotStarted, game.getState());
}

void GameTest::testFirstRevealStartsGame() {
    Game game(GameConfig::beginner());
    game.reveal({4, 4});
    CPPUNIT_ASSERT_EQUAL(GameState::InProgress, game.getState());
}

void GameTest::testFirstRevealIsNeverMine() {
    // Run many times to guard against lucky RNG
    for (int i = 0; i < 50; ++i) {
        Game game(GameConfig::beginner());
        RevealResult result = game.reveal({4, 4});
        CPPUNIT_ASSERT(!result.revealed.empty());
        CPPUNIT_ASSERT(!result.revealed[0].isMine);
        CPPUNIT_ASSERT(game.getState() != GameState::Lost);
    }
}

void GameTest::testRevealMine_gameLost() {
    // 10x10 board with 90 mines: every non-mine tile borders many mines so no
    // cascade occurs and winning in one click is impossible
    Game game(GameConfig::custom(10, 10, 90));
    game.reveal({0, 0});
    CPPUNIT_ASSERT_EQUAL(GameState::InProgress, game.getState());

    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 10; ++c) {
            Position p{r, c};
            if (game.getBoard().getTile(p).isMine()) {
                RevealResult result = game.reveal(p);
                CPPUNIT_ASSERT_EQUAL(GameState::Lost, result.newState);
                return;
            }
        }
    }
}

void GameTest::testRevealResult_containsClickedTile() {
    Game game(GameConfig::custom(5, 5, 1));
    RevealResult result = game.reveal({2, 2});
    CPPUNIT_ASSERT(!result.revealed.empty());
    bool found = false;
    for (const auto& t : result.revealed) {
        if (t.pos == Position{2, 2}) { found = true; break; }
    }
    CPPUNIT_ASSERT(found);
}

void GameTest::testRevealResult_floodFillReturnsMultipleTiles() {
    // 9x9 with 1 mine — first click on the center will almost certainly cascade
    Game game(GameConfig::custom(9, 9, 1));
    RevealResult result = game.reveal({4, 4});
    CPPUNIT_ASSERT((int)result.revealed.size() > 1);
}

void GameTest::testWinCondition() {
    Game game(GameConfig::custom(3, 3, 1));
    game.reveal({1, 1});

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (!game.getBoard().getTile({r, c}).isMine())
                game.reveal({r, c});
        }
    }

    CPPUNIT_ASSERT_EQUAL(GameState::Won, game.getState());
}

void GameTest::testNoActionsAfterGameOver() {
    Game game(GameConfig::custom(10, 10, 90));
    game.reveal({0, 0});
    CPPUNIT_ASSERT_EQUAL(GameState::InProgress, game.getState());

    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 10; ++c) {
            if (game.getBoard().getTile({r, c}).isMine()) {
                game.reveal({r, c});
                goto done;
            }
        }
    }
    done:

    CPPUNIT_ASSERT_EQUAL(GameState::Lost, game.getState());

    // Reveal after loss should return empty result and not change state
    RevealResult result = game.reveal({0, 0});
    CPPUNIT_ASSERT(result.revealed.empty());
    CPPUNIT_ASSERT_EQUAL(GameState::Lost, result.newState);
}

void GameTest::testFlag_noopWhenNotStarted() {
    Game game(GameConfig::beginner());
    game.flag({0, 0});
    CPPUNIT_ASSERT_EQUAL(TileState::Hidden, game.getBoard().getTile({0, 0}).getState());
}

void GameTest::testFlag_togglesOnBoard() {
    Game game(GameConfig::beginner());
    game.reveal({4, 4});
    game.flag({0, 0});
    CPPUNIT_ASSERT_EQUAL(TileState::Flagged, game.getBoard().getTile({0, 0}).getState());
    game.flag({0, 0});
    CPPUNIT_ASSERT_EQUAL(TileState::Hidden, game.getBoard().getTile({0, 0}).getState());
}

void GameTest::testReset() {
    Game game(GameConfig::beginner());
    game.reveal({4, 4});
    CPPUNIT_ASSERT_EQUAL(GameState::InProgress, game.getState());
    game.reset();
    CPPUNIT_ASSERT_EQUAL(GameState::NotStarted, game.getState());
    CPPUNIT_ASSERT_EQUAL(0, game.getBoard().getRevealedCount());
}
