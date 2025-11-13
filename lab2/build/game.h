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
            (void)game;
            (void)i;
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
        char Move(Game *game = nullptr, int i = 0);
};

class CPlayer : public Player
{
    public:
        CPlayer(std::string Name) : Player(Name) {};
        char Move(Game *game = nullptr, int i = 0);
};

class DPlayer : public Player
{
    public:
        DPlayer(std::string Name) : Player(Name) {};
        char Move(Game *game = nullptr, int i = 0);
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


int count_three(char a, char b, char c);
void CountScore(Game* Game_MAIN);
void Play(Game* Game_MAIN, std::string mode, int rounds = 1);
