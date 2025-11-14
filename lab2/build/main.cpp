#include "game.h"
#include <iostream>

int main()
{
    using namespace std;
    string players[] = {"C", "D", "TT", "D", "TT"};
    Game Main_Game(5, players);
    // cout<<Main_Game.GetPlayer(0)->Name()<<endl;
    Play(&Main_Game, "detailed");
    cout<<*(Main_Game.GetHistory())<<endl;
    int *score = Main_Game.GetScoreboard();
    for (int i = 0; i < Main_Game.GetAmount(); i++)
    {
        cout<<Main_Game.GetPlayer(i)->Name()<<": "<<score[i]<<endl;
    }
    
    return 0;
}
