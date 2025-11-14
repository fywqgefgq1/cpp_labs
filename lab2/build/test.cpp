#include <gtest/gtest.h>
#include "game.h"



TEST(GameTest, InitPlayers) 
{
    CPlayer cplayer("TestC");
    DPlayer dplayer("TestD");
    Random_Player rplayer("TestRandom");
    TTPlayer ttplayer("TestTT");

    EXPECT_EQ(cplayer.Name(), "TestC");
    EXPECT_EQ(dplayer.Name(), "TestD");
    EXPECT_EQ(rplayer.Name(), "TestRandom");
    EXPECT_EQ(ttplayer.Name(), "TestTT");
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
    Player** players = game.GetPlayers();
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
    Player** players = game.GetPlayers();
    for (int i = 0; i < 6; i++) 
    {
        EXPECT_EQ(scores[i], 0);
        Random_Player* randplr = dynamic_cast<Random_Player*>(players[i]);
        EXPECT_TRUE(randplr != nullptr);
    }
}

TEST(GameTest, GameInitializationAllParams) 
{
    std::string players_STRARR[] = {"Random", "C", "D", "TT"};
    Game game(4, players_STRARR);
    EXPECT_EQ(game.GetAmount(), 4);
    EXPECT_TRUE(game.GetHistory()->empty());
    
    int* scores = game.GetScoreboard();
    Player** players = game.GetPlayers();
    for (int i = 0; i < 4; i++) 
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

    EXPECT_EQ(game.GetPlayer(0)->Name(), "RandomPlayer_1");
    EXPECT_EQ(game.GetPlayer(1)->Name(), "CPlayer_2");
    EXPECT_EQ(game.GetPlayer(2)->Name(), "DPlayer_3");
    EXPECT_EQ(game.GetPlayer(3)->Name(), "TTPlayer_4");
}

TEST(GameTest, TTPlayerFirstMove) 
{
    Game game;
    TTPlayer player("TT");
    EXPECT_EQ(player.Move(&game, 0), 'C');
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

    char move = game.GetPlayer(0)->Move(&game, 3);
    EXPECT_EQ(move, 'D');
}

TEST(GameTest, CountThreeFunction) 
{
    EXPECT_EQ(count_three('C', 'C', 'C'), 7);
    EXPECT_EQ(count_three('D', 'D', 'D'), 1);
    EXPECT_EQ(count_three('C', 'C', 'D'), 3);
    EXPECT_EQ(count_three('D', 'D', 'C'), 5);
    EXPECT_EQ(count_three('C', 'D', 'D'), 0);
    EXPECT_EQ(count_three('D', 'C', 'C'), 9);
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
    std::string playerTypes[] = {"C", "D", "TT", "D", "TT"};
    Game game(5, playerTypes);
    Play(&game, "tournament", 5);
    EXPECT_EQ(game.GetHistory()->length(), 25);
    EXPECT_EQ(*(game.GetHistory()), "CDCDCCDCDCCDCDCCDCDCCDCDC");
    int* scores = game.GetScoreboard();
    EXPECT_EQ(scores[0], 95);
    EXPECT_EQ(scores[1], 210);
    EXPECT_EQ(scores[2], 95);
    EXPECT_EQ(scores[3], 210);
    EXPECT_EQ(scores[4], 95);
}
