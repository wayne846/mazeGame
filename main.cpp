#include <iostream>
#include <conio.h>
#include "game.h"

using namespace std;

int main(){
    char input = '0';
    World *world1 = new World();
    Player *player = new Player();

    WorldSetter::worldSetup(world1, player);

    while(input != 'z'){
        display(player);
        input = getch();
        if (input=='w' || input=='s' || input=='a' || input=='d') player->move(input);
        if (input=='e') player->interact();
    }
}
