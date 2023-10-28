#include <iostream>
#include <conio.h>
#include <vector>

using namespace std;

class Room{
    public:
        int width;
        int height;
        vector<char> content;

        Room(int width, int height){
            this->height = height;
            this->width = width;
            for (int i = 0; i < height*width; i++){
                if (i < width || i > height*width-width-1 || i%width==0 || (i+1)%width==0){
                    this->content.push_back('*');
                }else{
                    this->content.push_back(' ');
                }
            }
        }
};

class Player{
    public:
        int position;
        
        void move(char dir, Room *room){
            int bias = 0;
            if (dir == 'w') bias = -room->width;
            if (dir == 's') bias = room->width;
            if (dir == 'a') bias = -1;
            if (dir == 'd') bias = 1;
            if (room->content[position + bias] == ' '){
                position += bias;
            }
        }
};

void display(Room*, Player*);

int main(){
    char input = '0';
    Room room1(20, 10);
    Player player;
    player.position = room1.width+1;
    while(input != 'z'){
        display(&room1, &player);
        input = getch();
        player.move(input, &room1);
    }
    

}

void display(Room *room, Player *player){
    system("cls");
    for (int i = 0; i < room->width*room->height; i++){
        if (i == player->position){
            cout << '@' << ' ';
        }else{
            cout << room->content[i] << ' ';
        }
        if (i%room->width == room->width-1) cout << '\n';
    }
}