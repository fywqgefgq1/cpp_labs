#include "game.h"
#include <iostream>
#include <map>
#include <array>

char Random_Player::Move()
{
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 100);
    if (dist(rng) > 50)
    {
        return 'C';
    }
    return 'D';
}

char CPlayer::Move()
{
    return 'C';
}

char DPlayer::Move()
{
    return 'D';
}

char TTPlayer::MoveTT(Game *game, int i)
{
    int players_amount = game->GetAmount();
    std::string* history = game->GetHistory();
    int current_round = i/(players_amount);
    if (current_round == 0)
    {
        return 'C';
    }
    int decision = 0;
    for (int j = (current_round - 1) * players_amount; j < (current_round) * players_amount; j++)
    {
        if (i != j)
        {
            if ((*history)[j] == 'C')
            {
                decision += 1;
            }
            else if ((*history)[j] == 'D')
            {
                decision -= 1;
            }
        }
    }
    if (decision >= 0)
    {
        return 'C';
    }
    return 'D';
}

const std::map<int, std::array<int, 3>> Points_table = { 
        {0b000, {7,7,7}},
        {0b001, {3,3,9}},
        {0b010, {3,9,3}},
        {0b011, {0,5,5}},
        {0b100, {9,3,3}},
        {0b101, {5,0,5}},
        {0b110, {5,5,0}},
        {0b111, {1,1,1}},
};

void count_three(Game* Game_MAIN, int i, int j, int k)
{
    std::string *history = Game_MAIN->GetHistory();
    int* scoreboard = Game_MAIN->GetScoreboard();
    int playerAmount = Game_MAIN->GetAmount();
    int curPlay = (((*history)[i] == 'D') << 2) | 
    (((*history)[j] == 'D') << 1) | 
    (((*history)[k] == 'D'));
    std::array<int, 3> result = Points_table.at(curPlay);
    scoreboard[i%playerAmount] += result[0];
    scoreboard[j%playerAmount] += result[1];
    scoreboard[k%playerAmount] += result[2];
}

void CountScore(Game* Game_MAIN)
{
    std::string *history = Game_MAIN->GetHistory();
    int len = history->length();
    int i = 0;
    int players_amount = Game_MAIN->GetAmount();
    while (i <= len/players_amount - 1)
    {
        //std::cout<<"ROUND "<<i + 1<<std::endl;
        int offset = i*players_amount;
        for (int j = 0; j < players_amount - 2; j++)
        {
            for (int k = j + 1; k < players_amount - 1; k++)
            {
                for (int l = k + 1; l < players_amount; l++)
                {
                    count_three(Game_MAIN, j + offset, k + offset, l + offset);
                    //std::cout<<(*history)[j + offset]<<" vs "<<(*history)[k + offset]<<" vs "<<(*history)[l + offset]<<std::endl;
                    // score[j] += count_three((*history)[j + offset], (*history)[k + offset], (*history)[l + offset]);
                    // score[k] += count_three((*history)[k + offset], (*history)[j + offset], (*history)[l + offset]);
                    // score[l] += count_three((*history)[l + offset], (*history)[j + offset], (*history)[k + offset]);
                }
            }
        }
        i++;
    }
}

void Play(Game* Game_MAIN, std::string mode, int rounds)
{
    Player** Players = Game_MAIN->GetPlayers();
    std::string *history = Game_MAIN->GetHistory();
    int amount_of_players = Game_MAIN->GetAmount();
    std::cout<<amount_of_players<<" Player Game "<<rounds<<" rounds"<<std::endl;
    if (mode == "detailed")
    {
        int i = 0;
        std::string status = "";
        while (status != "quit")
        {
            std::cout<<"Move "<<i + 1<<", "<<Game_MAIN->GetPlayer(i%amount_of_players)->Name()<<" : "<<(*history)<<" ";
            std::getline(std::cin, status);
            Player* curPlayer = Players[i%(amount_of_players)];
            TTPlayer* ttplr = dynamic_cast<TTPlayer*>(curPlayer);
            if (ttplr)
            {
                *history += ttplr->MoveTT(Game_MAIN, i);
            }
            else
            {
                *history += curPlayer->Move();
            }
            i++;
        }
        CountScore(Game_MAIN);
    }
    else if (mode == "fast")
    {
        for (int i = 0; i < rounds * amount_of_players; i++)
        {
            Player* curPlayer = Players[i%(amount_of_players)];
            TTPlayer* ttplr = dynamic_cast<TTPlayer*>(curPlayer);
            if (ttplr)
            {
                *history += ttplr->MoveTT(Game_MAIN, i);
            }
            else
            {
                *history += curPlayer->Move();
            }
        }
        CountScore(Game_MAIN);
    }
    else if (mode == "tournament")
    {
        Game temp_game(3);
        int* temp_scoreboard;
        int* scoreboard = Game_MAIN->GetScoreboard();
        Player** temp_game_players = temp_game.GetPlayers();
        for (int j = 0; j < amount_of_players - 2; j++)
        {
            for (int k = j + 1; k < amount_of_players - 1; k++)
            {
                for (int l = k + 1; l < amount_of_players; l++)
                {
                    temp_game_players[0] = Players[j];
                    temp_game_players[1] = Players[k];
                    temp_game_players[2] = Players[l];
                    Play(&temp_game, "fast", rounds);

                    temp_scoreboard = temp_game.GetScoreboard();
                    scoreboard[j] += temp_scoreboard[0];
                    scoreboard[k] += temp_scoreboard[1];
                    scoreboard[l] += temp_scoreboard[2];

                    temp_game.ClearScoreboard();
                    temp_game.ClearHistory();
                }
            }
        }
    }
    return;
}


