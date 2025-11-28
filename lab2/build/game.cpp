#include "game.h"
#include <iostream>
#include <map>
#include <array>
#include <vector>
#include <fstream>

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

char FTTPlayer::MoveFTT(Game *game, int i)
{
    int players_amount = game->GetAmount();
    std::string* history = game->GetHistory();
    int current_round = i/(players_amount);
    if (current_round == 0)
    {
        return 'C';
    }
    int c_amount = 0;
    int d_amount = 0;
    for (int j = (current_round - 1) * players_amount; j < (current_round) * players_amount; j++)
    {
        if (i != j)
        {
            if ((*history)[j] == 'C')
            {
                c_amount++;
            }
            else if ((*history)[j] == 'D')
            {
                d_amount++;
            }
        }
    }
    if (c_amount == 0)
    {
        return 'C';
    }
    if (d_amount == 1) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 100);
        if (dist(gen) <= 25) 
        {
            return 'C';
        }
    }
    return 'D';
}

void AnalyticPlayer::PlayerCombos(Game *game, std::vector<std::array<int, 3>>* combo_table)
{
    int players_amount = game->GetAmount();
    //std::cout<<players_amount<<std::endl;
    for (int j = 0; j < players_amount - 2; j++)
    {
        for (int k = j + 1; k < players_amount - 1; k++)
        {
            for (int l = k + 1; l < players_amount; l++)
            {
                combo_table->push_back({j,k,l});
            }
        }
    }
    return;
}

void AnalyticPlayer::PlayersCRate(Game *game, int i, int cur_game)
{
    int players_amount = game->GetAmount();
    std::string *history = game->GetHistory();

    if (static_cast<int>(CRates.size()) != players_amount)
    {
        CRates.resize(players_amount);
        cooperation_count.resize(players_amount);
        appearance_count.resize(players_amount);
    }

    std::vector<std::array<int, 3>> combo_table;
    PlayerCombos(game, &combo_table);

    int aplr_index = combo_table[cur_game][i%3];
    int length = history->length();
    int h_index = length - aplr_index;
    const auto& combo = combo_table[cur_game];
    //std::cout<<h_index<<"______"<<length<<std::endl;
    int j = combo[0];
    int k = combo[1]; 
    int l = combo[2];
    if (h_index + 2 < length) 
    {
        if ((*history)[h_index] == 'C') cooperation_count[j]++;
        if ((*history)[h_index + 1] == 'C') cooperation_count[k]++;
        if ((*history)[h_index + 2] == 'C') cooperation_count[l]++;
        
        appearance_count[j]++;
        appearance_count[k]++;
        appearance_count[l]++;
        
        h_index += 3;
    }
    //std::cout<<"------"<<std::endl;
    for (int idx = 0; idx < players_amount; idx++)
    {
        if (appearance_count[idx] == 0 || idx == aplr_index)
        {
            CRates[idx] = 0;
        }
        else
        {
            CRates[idx] = (double)(cooperation_count[idx])/(double)(appearance_count[idx]);
        }
    }
    //std::cout<<"------\n"<<std::endl;
    return;
}

std::string PlayerClass(Player* player)
{
    if (player->Name()[0] == 'D')
    {
        return "D";
    }
    else if (player->Name()[0] == 'C')
    {
        return "C";
    }
    else if (player->Name()[0] == 'T')
    {
        return "TT";
    }
    else if (player->Name()[0] == 'A')
    {
        return "A";
    }
    else if (player->Name()[0] == 'F')
    {
        return "FTT";
    }
    else if (player->Name()[0] == 'M')
    {
        return "Meta";
    }
    return "Random";
}

char MetaPlayer::MoveMeta(Game *game, int i)
{
    std::string temp_game_mode = "";
    int rounds_amount = 0;
    // std::getline(configfile, temp_game_mode);
    std::ifstream configfile(configfileDir);
    configfile>>temp_game_mode;
    configfile>>rounds_amount;
    //std::cout<<temp_game_mode<<" "<<rounds_amount<<std::endl;
    configfile.close();
    if (temp_game_mode == "" || rounds_amount <= 0)
    {
        temp_game_mode = "fast";
        rounds_amount = 1;
    }
    std::vector<std::string> temp_Players;
    std::vector<Player*>& Players = game->GetPlayers();
    std::string pclass;
    int mplr_index = i%game->GetAmount();
    //std::cout<<mplr_index<<std::endl;
    for (int i = 0; i < game->GetAmount(); i++)
    {
        if (i != mplr_index)
        {
            pclass = PlayerClass(Players[i]);
            if (pclass == "Meta")
            {
                pclass = "Random";
            }
            temp_Players.push_back(pclass);
        }
    }

    Game temp_game(2, temp_Players.data());
    std::string* history = temp_game.GetHistory();
    Play(&temp_game, temp_game_mode, rounds_amount, 0, nullptr);
    int decision = 0;
    int length = history->length();
    for (int i = 0; i < length; i++)
    {
        if ((*history)[i] == 'C')
        {
            decision++;
            continue;
        }
        decision--;
    }
    if (decision >= 0)
    {
        return 'C';
    }
    return 'D';
}

char AnalyticPlayer::MoveAnalytic(Game* tournament, int i, int cur_game)
{
    //std::cout<<"CURGAME: "<<cur_game<<std::endl;
    if (cur_game == 0)
    {
        return 'C';
    }
    std::vector<std::array<int, 3>> combo_table;
    PlayerCombos(tournament, &combo_table);
    PlayersCRate(tournament, i, cur_game);
    double avg_CRate = (CRates[combo_table[cur_game][0]] 
        + CRates[combo_table[cur_game][1]] 
        + CRates[combo_table[cur_game][2]])/3;
    //std::cout<<"NIGGER"<<avg_CRate<<std::endl;
    if (avg_CRate >= 0.5)
    {
        return 'C';
    }
    //std::cout<<"AVGCRATE: "<<avg_CRate<<std::endl;
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
        if (players_amount < 3)
        {
            return;
        }
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

void GamePlayerMove(Game* Game_MAIN, int i, int current_game, Game* tournament = nullptr)
{
    std::vector<Player*>& Players = Game_MAIN->GetPlayers();
    std::string *history = Game_MAIN->GetHistory();
    int amount_of_players = Game_MAIN->GetAmount();

    Player* curPlayer = Players[i%(amount_of_players)];
    TTPlayer* ttplr = dynamic_cast<TTPlayer*>(curPlayer);
    FTTPlayer* fttplr = dynamic_cast<FTTPlayer*>(curPlayer);
    AnalyticPlayer* anplr = dynamic_cast<AnalyticPlayer*>(curPlayer);
    MetaPlayer* mplr = dynamic_cast<MetaPlayer*>(curPlayer);
    std::cout << "Move " << i << " - Player: " << Game_MAIN->GetPlayer(i % Game_MAIN->GetAmount())->Name() << std::endl;
    if (ttplr)
    {
        *history += ttplr->MoveTT(Game_MAIN, i);
    }
    else if (fttplr)
    {
        *history += fttplr->MoveFTT(Game_MAIN, i);
    }
    else if (anplr)
    {
        if (tournament)
        {
            *history += anplr->MoveAnalytic(tournament, i, current_game);
        }
        else
        {
            *history += anplr->MoveAnalytic(Game_MAIN, i, current_game);
        }
    }
    else if (mplr)
    {
        *history += mplr->MoveMeta(Game_MAIN, i);
    }
    else
    {
        *history += curPlayer->Move();
    }
}

void Play(Game* Game_MAIN, std::string mode, int rounds, int current_game, Game* tournament)
{
    std::vector<Player*>& Players = Game_MAIN->GetPlayers();
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
            GamePlayerMove(Game_MAIN, i, current_game);
            i++;
        }
        CountScore(Game_MAIN);
    }
    else if (mode == "fast")
    {
        for (int i = 0; i < rounds * amount_of_players; i++)
        {
            GamePlayerMove(Game_MAIN, i, current_game, tournament);
        }
        CountScore(Game_MAIN);
    }
    else if (mode == "tournament")
    {
        int tournament_index = 0;
        Game_MAIN->ClearScoreboard();
        int* temp_scoreboard;
        std::string* temp_history;
        int* scoreboard = Game_MAIN->GetScoreboard();
        for (int j = 0; j < amount_of_players - 2; j++)
        {
            for (int k = j + 1; k < amount_of_players - 1; k++)
            {
                for (int l = k + 1; l < amount_of_players; l++)
                {
                    std::array<std::string, 3> PlayerClasses;
                    PlayerClasses[0] = PlayerClass(Players[j]);
                    PlayerClasses[1] = PlayerClass(Players[k]);
                    PlayerClasses[2] = PlayerClass(Players[l]);
                    Game temp_game(3, PlayerClasses.data());
                    Play(&temp_game, "fast", rounds, tournament_index, Game_MAIN);

                    temp_scoreboard = temp_game.GetScoreboard();
                    temp_history = temp_game.GetHistory();
                    //std::cout<<j<<k<<l<<std::endl;
                    //std::cout<<temp_scoreboard[0]<<" "<<temp_scoreboard[1]<<" "<<temp_scoreboard[2]<<std::endl;
                    scoreboard[j] += temp_scoreboard[0];
                    scoreboard[k] += temp_scoreboard[1];
                    scoreboard[l] += temp_scoreboard[2];
                    *history += *temp_history;
                    tournament_index++;

                    temp_game.ClearScoreboard();
                    temp_game.ClearHistory();
                }
            }
        }
    }
    return;
}


