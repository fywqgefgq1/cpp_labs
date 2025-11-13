#include "game.h"
#include <iostream>

char Random_Player::Move(Game *game, int i)
{
    (void)game;
    (void)i;
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 100);
    if (dist(rng) > 50)
    {
        return 'C';
    }
    return 'D';
}

char CPlayer::Move(Game *game, int i)
{
    (void)game;
    (void)i;
    return 'C';
}

char DPlayer::Move(Game *game, int i)
{
    (void)game;
    (void)i;
    return 'D';
}

char TTPlayer::Move(Game *game, int i)
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
        if (i != j%players_amount)
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

int count_three(char a, char b, char c)
{
    //std::cout<<a<<" vs "<<b<<" vs "<<c<<std::endl;
    if (a == b && b == c)
    {
        if (a == 'C')
        {
            return 7;
        }
        return 1;
    }
    else if (a == b || a == c)
    {
        if (a == 'C')
        {
            return 3;
        }
        return 5;
    }
    else if (a == 'C')
    {
        return 0;
    }
    return 9;
}

void CountScore(Game* Game_MAIN)
{
    int *score = Game_MAIN->GetScoreboard();
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
                    //std::cout<<(*history)[j + offset]<<" vs "<<(*history)[k + offset]<<" vs "<<(*history)[l + offset]<<std::endl;
                    score[j] += count_three((*history)[j + offset], (*history)[k + offset], (*history)[l + offset]);
                    score[k] += count_three((*history)[k + offset], (*history)[j + offset], (*history)[l + offset]);
                    score[l] += count_three((*history)[l + offset], (*history)[j + offset], (*history)[k + offset]);
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
    std::cout<<amount_of_players<<" Player Game"<<std::endl;
    if (mode == "detailed")
    {
        int i = 0;
        std::string status = "";
        while (status != "quit")
        {
            std::cout<<"Move "<<i + 1<<", "<<Game_MAIN->GetPlayer(i%amount_of_players)->Name()<<" : "<<(*history)<<" ";
            std::getline(std::cin, status);
            *history += Players[i%(amount_of_players)]->Move(Game_MAIN, i);
            i++;
        }
    }
    if (mode == "fast" || mode == "tournament")
    {
        for (int i = 0; i < rounds * amount_of_players; i++)
        {
            *history += Players[i%(amount_of_players)]->Move(Game_MAIN, i);
        }
    }
    CountScore(Game_MAIN);
    return;
}


