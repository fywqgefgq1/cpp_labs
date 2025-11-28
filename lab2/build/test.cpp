#include <gtest/gtest.h>
#include "game.h"
#include <array>
#include <vector>

TEST(GameTest, InitPlayers) 
{
    CPlayer cplayer("TestC");
    DPlayer dplayer("TestD");
    Random_Player rplayer("TestRandom");
    TTPlayer ttplayer("TestTT");
    AnalyticPlayer aplayer("TestA");
    FTTPlayer fttplayer("TestFTT");

    EXPECT_EQ(cplayer.Name(), "TestC");
    EXPECT_EQ(dplayer.Name(), "TestD");
    EXPECT_EQ(rplayer.Name(), "TestRandom");
    EXPECT_EQ(ttplayer.Name(), "TestTT");
    EXPECT_EQ(fttplayer.Name(), "TestFTT");
    EXPECT_EQ(aplayer.Name(), "TestA");
}

TEST(GameTest, CPlayerMoveNoGame) 
{
    CPlayer cplayer("C");
    EXPECT_EQ(cplayer.Move(), 'C');
}

TEST(GameTest, DPlayerMoveNoGame) 
{
    DPlayer dplayer("D");
    EXPECT_EQ(dplayer.Move(), 'D');
}

TEST(GameTest, RPlayerMoveNoGame) 
{
    Random_Player rplayer("D");
    char move = rplayer.Move();
    EXPECT_TRUE(move == 'C' || move == 'D');
    move = rplayer.Move();
    EXPECT_TRUE(move == 'C' || move == 'D');
}

TEST(GameTest, GameInitializationNoParams) 
{
    Game game;
    EXPECT_EQ(game.GetAmount(), 3);
    EXPECT_TRUE(game.GetHistory()->empty());
    
    int* scores = game.GetScoreboard();
    std::vector<Player*> players = game.GetPlayers();
    for (int i = 0; i < 3; i++) 
    {
        EXPECT_EQ(scores[i], 0);
        Random_Player* randplr = dynamic_cast<Random_Player*>(players[i]);
        EXPECT_TRUE(randplr != nullptr);
    }
}

TEST(GameTest, GameInitializationAmountParam) 
{
    Game game(6);
    EXPECT_EQ(game.GetAmount(), 6);
    EXPECT_TRUE(game.GetHistory()->empty());
    
    int* scores = game.GetScoreboard();
    std::vector<Player*> players = game.GetPlayers();
    for (int i = 0; i < 6; i++) 
    {
        EXPECT_EQ(scores[i], 0);
        Random_Player* randplr = dynamic_cast<Random_Player*>(players[i]);
        EXPECT_TRUE(randplr != nullptr);
    }
}

TEST(GameTest, GameInitializationAllParams) 
{
    std::string players_STRARR[] = {"Random", "C", "D", "TT", "A", "FTT", "Meta"};
    Game game(7, players_STRARR);
    EXPECT_EQ(game.GetAmount(), 7);
    EXPECT_TRUE(game.GetHistory()->empty());
    
    int* scores = game.GetScoreboard();
    std::vector<Player*> players = game.GetPlayers();
    for (int i = 0; i < 7; i++) 
    {
        EXPECT_EQ(scores[i], 0);
    }
    Random_Player* randplr = dynamic_cast<Random_Player*>(players[0]);
    EXPECT_TRUE(randplr != nullptr);
    CPlayer* cplr = dynamic_cast<CPlayer*>(players[1]);
    EXPECT_TRUE(cplr != nullptr);
    DPlayer* dplr = dynamic_cast<DPlayer*>(players[2]);
    EXPECT_TRUE(dplr != nullptr);
    TTPlayer* ttplr = dynamic_cast<TTPlayer*>(players[3]);
    EXPECT_TRUE(ttplr != nullptr);
    AnalyticPlayer* anplr = dynamic_cast<AnalyticPlayer*>(players[4]);
    EXPECT_TRUE(anplr!= nullptr);
    FTTPlayer* fttplr = dynamic_cast<FTTPlayer*>(players[5]);
    EXPECT_TRUE(fttplr!= nullptr);
    MetaPlayer* mplr = dynamic_cast<MetaPlayer*>(players[6]);
    EXPECT_TRUE(mplr!= nullptr);

    EXPECT_EQ(game.GetPlayer(0)->Name(), "RandomPlayer_1");
    EXPECT_EQ(game.GetPlayer(1)->Name(), "CPlayer_2");
    EXPECT_EQ(game.GetPlayer(2)->Name(), "DPlayer_3");
    EXPECT_EQ(game.GetPlayer(3)->Name(), "TTPlayer_4");
    EXPECT_EQ(game.GetPlayer(4)->Name(), "AnalyticPlayer_5");
    EXPECT_EQ(game.GetPlayer(5)->Name(), "FTTPlayer_6");
    EXPECT_EQ(game.GetPlayer(6)->Name(), "MetaPlayer_7");
}

TEST(GameTest, TTPlayerFirstMove) 
{
    Game game;
    TTPlayer player("TT");
    EXPECT_EQ(player.MoveTT(&game, 0), 'C');
}

TEST(GameTest, FTTPlayerFirstMove) 
{
    Game game;
    FTTPlayer player("FTT");
    EXPECT_EQ(player.MoveFTT(&game, 0), 'C');
}

TEST(GameTest, AnalyticPlayerFirstMove) {
    AnalyticPlayer player("TestAnalytic");
    std::string players[] = {"C", "D", "A"};
    Game game(3, players);
    char move = player.MoveAnalytic(&game, 0, 0);
    EXPECT_EQ(move, 'C');
}

TEST(GameTest, TTPlayerGameNoHistory) 
{
    std::string players_STRARR[] = {"D", "D", "TT"};
    Game game(3, players_STRARR);
    Play(&game, "fast", 3);
    EXPECT_EQ(*(game.GetHistory()), "DDCDDDDDD");
}

TEST(GameTest, TTPlayerExistingHistory) 
{
    std::string players_STRARR[] = {"TT", "Random", "Random"};
    Game game(3, players_STRARR);

    std::string* history = game.GetHistory();
    *history = "CDD";

    TTPlayer* testplr = dynamic_cast<TTPlayer*>(game.GetPlayer(0));
    char move = testplr->MoveTT(&game, 3);
    EXPECT_EQ(move, 'D');
}

TEST(GameTest, FTTPlayerExistingHistory) 
{
    std::string players_STRARR[] = {"FTT", "Random", "Random"};
    Game game(3, players_STRARR);

    std::string* history = game.GetHistory();
    *history = "CDD";

    FTTPlayer* testplr = dynamic_cast<FTTPlayer*>(game.GetPlayer(0));
    char move = testplr->MoveFTT(&game, 3);
    EXPECT_EQ(move, 'D');
}



TEST(GameTest, PlayerCombosThreePlayers) 
{
    AnalyticPlayer player("AnalyticTest");
    Game game(3);
    std::vector<std::array<int, 3>> combo_table;
    player.PlayerCombos(&game, &combo_table);
    EXPECT_EQ(combo_table.size(), 1);
    EXPECT_EQ(combo_table[0], (std::array<int, 3>{0, 1, 2}));
}

TEST(GameTest, PlayerCombosGeneration) 
{
    AnalyticPlayer player("AnalyticTest");
    Game game(4);
    std::vector<std::array<int, 3>> combo_table;
    
    player.PlayerCombos(&game, &combo_table);
    EXPECT_EQ(combo_table.size(), 4);
    EXPECT_EQ(combo_table[0], (std::array<int, 3>{0, 1, 2}));
    EXPECT_EQ(combo_table[1], (std::array<int, 3>{0, 1, 3}));
    EXPECT_EQ(combo_table[2], (std::array<int, 3>{0, 2, 3}));
    EXPECT_EQ(combo_table[3], (std::array<int, 3>{1, 2, 3}));
}

TEST(GameTest, CountFunc)
{
    Game game(3);
    std::string* history = game.GetHistory();
    int* scoreboard = game.GetScoreboard();
    std::map<std::string, std::array<int, 3>> checkTable = {
        {"CCC", {7,7,7}},
        {"CCD", {3,3,9}},
        {"CDC", {3,9,3}},
        {"CDD", {0,5,5}},
        {"DCC", {9,3,3}},
        {"DCD", {5,0,5}},
        {"DDC", {5,5,0}},
        {"DDD", {1,1,1}},
    };
    for (std::pair<const std::string, std::array<int, 3>> set : checkTable)
    {
        *history = set.first;
        scoreboard[0] = 0;
        scoreboard[1] = 0;
        scoreboard[2] = 0;
        count_three(&game, 0, 1, 2);
        EXPECT_EQ(scoreboard[0], set.second[0]);
        EXPECT_EQ(scoreboard[1], set.second[1]);
        EXPECT_EQ(scoreboard[2], set.second[2]);
    }

}

TEST(GameTest, ScoreCounting) 
{
    std::string playerTypes[] = {"C", "D", "C"};
    Game game(3, playerTypes);
    std::string* history = game.GetHistory();
    *history = "CDCDDCCDD";
    
    CountScore(&game);
    
    int* scores = game.GetScoreboard();
    EXPECT_EQ(scores[0], 8);
    EXPECT_EQ(scores[1], 19);
    EXPECT_EQ(scores[2], 8);
}

TEST(GameTest, PlayFast) 
{
    std::string playerTypes[] = {"C", "D", "C"};
    Game game(3, playerTypes);
    Play(&game, "fast", 5);
    EXPECT_EQ(game.GetHistory()->length(), 15);
    EXPECT_EQ(*(game.GetHistory()), "CDCCDCCDCCDCCDC");
    int* scores = game.GetScoreboard();
    EXPECT_EQ(scores[0], 15);
    EXPECT_EQ(scores[1], 45);
    EXPECT_EQ(scores[2], 15);
}

TEST(GameTest, PlayTournament) 
{
    std::string playerTypes[] = {"C", "D", "TT", "A"};
    Game game(4, playerTypes);
    Play(&game, "tournament", 2, 0, nullptr);
    EXPECT_EQ(*(game.GetHistory()), "CDCCDCCDDCDDCCDCCDDCCDCC");
    int* scores = game.GetScoreboard();
    EXPECT_EQ(scores[0], 12);
    EXPECT_EQ(scores[1], 46);
    EXPECT_EQ(scores[2], 18);
    EXPECT_EQ(scores[3], 34);
}

TEST(GameTest, MetaPlayerGameTest) 
{
    std::string playerTypes[] = {"Random", "Random", "Meta"};
    Game testGame(3, playerTypes);
    MetaPlayer* metaPlayer = dynamic_cast<MetaPlayer*>(testGame.GetPlayer(2));
    metaPlayer->SetConfigDir("test_config.txt");

    char move = metaPlayer->MoveMeta(&testGame, 0);
    EXPECT_TRUE(move == 'C' || move == 'D');
}

TEST(GameTest, ULTIMATE_TEST)
{
    std::string players[] = {"C", "D", "TT", "A", "Meta"};
    Game game(5, players);
    Play(&game, "tournament", 5);
    EXPECT_EQ(*(game.GetHistory()), "CDCCDCCDCCDCCDCCDDCDDCDDCDDCDDCDCCDCCDCCDCCDCCCDCCDCCDCCDCCDCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCDCCDCCDCCDCCDCCDCCDCCDCCDCCDCCDDCDDCDDCDDCDDCCDCCDCCDCCDCCDC");
    int* scores = game.GetScoreboard();
    EXPECT_EQ(scores[0], 115);
    EXPECT_EQ(scores[1], 230);
    EXPECT_EQ(scores[2], 110);
    EXPECT_EQ(scores[3], 190);
    EXPECT_EQ(scores[4], 115);
}
