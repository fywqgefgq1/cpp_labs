#include <iostream>
#include <random>

class Game;

class Player
{
    private:
        int score;
        std::string name;
    public:
        Player(std::string NAME)
        {
            score = 0;
            name = NAME;
        }
        Player()
        {
            score = 0;
            name = "Player";
        }
        std::string Name()
        {
            return name;
        }
        int Score()
        {
            return score;
        }
        virtual char Move(Game *game = nullptr, int i = 0) 
        {
            return 'N';
        }
};

class Random_Player : public Player
{
    private:
        std::mt19937 rng;
    public:
        Random_Player(std::string Name) : Player(Name) 
        {
            std::random_device dev;
            rng.seed(dev());
        }
        char Move(Game *game = nullptr, int i = 0)
        {
            std::uniform_int_distribution<std::mt19937::result_type> dist(1, 100);
            if ( dist(rng) > 50 )
            {
                return 'C';
            }
            return 'D';
        }
};

class CPlayer : public Player
{
    public:
        CPlayer(std::string Name) : Player(Name) {};
        char Move(Game *game = nullptr, int i = 0)
        {
            return 'C';
        }
};

class DPlayer : public Player
{
    public:
        DPlayer(std::string Name) : Player(Name) {};
        char Move(Game *game = nullptr, int i = 0)
        {
            return 'D';
        }
};

class TTPlayer : public Player
{
    public:
        TTPlayer(std::string Name) : Player(Name) {};
        char Move(Game *game, int i);
};

class Game
{
    private:
        Player** Players = nullptr;
        int* scoreboard = nullptr;
        std::string history = "";
        int playersAmount = 3;

        void Init(std::string *PlayerTypes = nullptr)
        {
            Players = new Player* [playersAmount];
            scoreboard = new int[playersAmount];
            std::string name = "";
            if (PlayerTypes)
            {
                for (int i = 0; i < playersAmount; i++)
                {
                    if (PlayerTypes[i] == "Random")
                    {
                        name = "RandomPlayer_" + std::to_string(i+1);
                        Players[i] = new Random_Player(name);
                    }
                    else if (PlayerTypes[i] == "C")
                    {
                        name = "CPlayer_" + std::to_string(i+1);
                        Players[i] = new CPlayer(name);
                    }
                    else if (PlayerTypes[i] == "D")
                    {
                        name = "DPlayer_" + std::to_string(i+1);
                        Players[i] = new DPlayer(name);
                    }
                    else if (PlayerTypes[i] == "TT")
                    {
                        name = "TTPlayer_" + std::to_string(i+1);
                        Players[i] = new TTPlayer(name);
                    }
                    scoreboard[i] = 0;
                }
            }
            else
            {
                for (int i = 0; i < playersAmount; i++)
                {
                    name = "RandomPlayer_" + std::to_string(i+1);
                    Players[i] = new Random_Player(name);
                }
            }
        }
    public:
        Game()
        {
            Init();
        }

        Game(int a)
        {
            if (a > 1)
            {
                playersAmount = a;
            }
            Init();
        }

        Game(int a, std::string *PlayerTypes)
        {
            if (a > 1)
            {
                playersAmount = a;
            }
            Init(PlayerTypes);
        }

        Player* GetPlayer(int index)
        {
            return Players[index];
        }
        Player** GetPlayers()
        {
            return Players;
        }
        int* GetScoreboard()
        {
            return scoreboard;
        }
        std::string* GetHistory()
        {
            return &history;
        }
        int GetAmount()
        {
            return playersAmount;
        }
};

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
    for (int j = current_round * players_amount; j < (current_round + 1) * players_amount; j++)
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

int count_three(char a, char b, char c)
{
    //std::cout<<a<<" vs "<<b<<std::endl;
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
                    score[l] += count_three((*history)[l + offset], (*history)[k + offset], (*history)[j + offset]);
                }
            }
        }
        i++;
    }
}

void Play(Game* Game_MAIN, std::string mode, int rounds = 1)
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

int main()
{
    using namespace std;
    string players[] = {"Random", "Random", "Random", "TT", "TT", "TT"};
    Game Main_Game(6, players);
    // cout<<Main_Game.GetPlayer(0)->Name()<<endl;
    Play(&Main_Game, "tournament", 3);
    cout<<*(Main_Game.GetHistory())<<endl;
    int *score = Main_Game.GetScoreboard();
    for (int i = 0; i < Main_Game.GetAmount(); i++)
    {
        cout<<Main_Game.GetPlayer(i)->Name()<<": "<<score[i]<<endl;
    }
    
    return 0;
}
