#include <iostream>
#include <random>
#include <string>

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
        virtual ~Player() = default;
        std::string Name()
        {
            return name;
        }
        int Score()
        {
            return score;
        }
        virtual char Move() 
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
        char Move();
};

class CPlayer : public Player
{
    public:
        CPlayer(std::string Name) : Player(Name) {};
        char Move();
};

class DPlayer : public Player
{
    public:
        DPlayer(std::string Name) : Player(Name) {};
        char Move();
};

class TTPlayer : public Player
{
    public:
        TTPlayer(std::string Name) : Player(Name) {};
        char MoveTT(Game *game, int i);
};

class FTTPlayer : public Player
{
    public:
        FTTPlayer(std::string Name) : Player(Name) {};
        char MoveFTT(Game *game, int i);
};

class AnalyticPlayer : public Player
{
    private:
        std::vector<double> CRates;
        std::vector<int> cooperation_count;
        std::vector<int> appearance_count;
    public:
        AnalyticPlayer(std::string Name) : Player(Name) 
        {
            CRates = std::vector<double>();
            cooperation_count = std::vector<int>();
            appearance_count = std::vector<int>();
        };
        void PlayersCRate(Game *game, int i, int cur_game = 0);
        void PlayerCombos(Game *game, std::vector<std::array<int, 3>>* combo_table);
        char MoveAnalytic(Game* tournament_game, int i, int current_game = 0);
};

class MetaPlayer : public Player
{
    private:
        std::string configfileDir;
    public:
        MetaPlayer(std::string Name) : Player(Name) {};
        MetaPlayer(std::string Name, std::string dir) : Player(Name) 
        {
            SetConfigDir(dir);
        };
        char MoveMeta(Game *game, int i);
        void SetConfigDir(std::string new_dir)
        {
            configfileDir = new_dir;
        }
};

class Game
{
    private:
        std::vector<Player*> Players;
        // Player** Players = nullptr;
        int* scoreboard = nullptr;
        std::string history = "";
        int playersAmount = 3;

        void Init(std::string *PlayerTypes = nullptr)
        {
            scoreboard = new int[playersAmount];
            std::string name = "";
            if (PlayerTypes)
            {
                for (int i = 0; i < playersAmount; i++)
                {
                    if (PlayerTypes[i] == "Random")
                    {
                        name = "RandomPlayer_" + std::to_string(i+1);
                        Players.push_back(new Random_Player(name));
                    }
                    else if (PlayerTypes[i] == "C")
                    {
                        name = "CPlayer_" + std::to_string(i+1);
                        Players.push_back(new CPlayer(name));
                    }
                    else if (PlayerTypes[i] == "D")
                    {
                        name = "DPlayer_" + std::to_string(i+1);
                        Players.push_back(new DPlayer(name));
                    }
                    else if (PlayerTypes[i] == "TT")
                    {
                        name = "TTPlayer_" + std::to_string(i+1);
                        Players.push_back(new TTPlayer(name));
                    }
                    else if (PlayerTypes[i] == "A")
                    {
                        name = "AnalyticPlayer_" + std::to_string(i+1);
                        Players.push_back(new AnalyticPlayer(name));
                    }
                    else if (PlayerTypes[i] == "FTT")
                    {
                        name = "FTTPlayer_" + std::to_string(i+1);
                        Players.push_back(new FTTPlayer(name));
                    }
                    else if (PlayerTypes[i] == "Meta")
                    {
                        name = "MetaPlayer_" + std::to_string(i+1);
                        Players.push_back(new MetaPlayer(name));
                    }
                    scoreboard[i] = 0;
                }
            }
            else
            {
                for (int i = 0; i < playersAmount; i++)
                {
                    name = "RandomPlayer_" + std::to_string(i+1);
                    Players.push_back(new Random_Player(name));
                    scoreboard[i] = 0;
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
        std::vector<Player*>& GetPlayers()
        {
            return Players;
        }
        int* GetScoreboard()
        {
            return scoreboard;
        }
        void SetPlayer(int index, Player* player)
        {
            if (index >= 0 && static_cast<size_t>(index) < Players.size()) 
            {
                delete Players[index];
                Players[index] = player;
            }
        }
        void ClearScoreboard()
        {
            for (int i = 0; i < playersAmount; i++)
            {
                scoreboard[i] = 0;
            }
            return;
        }
        void ClearHistory()
        {
            history = "";
            return;
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


void count_three(Game* Game_MAIN, int i, int j, int k);
void CountScore(Game* Game_MAIN);
void Play(Game* Game_MAIN, std::string mode, int rounds = 1, int current_game = 0, Game* tournament = nullptr);
