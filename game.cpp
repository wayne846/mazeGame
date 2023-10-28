#include <iostream>
#include <conio.h>
#include <vector>

using namespace std;

class World;
class Room;
class Player;

class Room{
    private:
        int width;
        int height;
        vector<char> content;

    public:
        int number;
        int upRoomNumber;
        int downRoomNumber;
        int leftRoomNumber;
        int rightRoomNumber;

        Room(int width, int height, int number){
            this->height = height;
            this->width = width;
            this->number = number;
            this->upRoomNumber = 0;
            this->downRoomNumber = 0;
            this->leftRoomNumber = 0;
            this->rightRoomNumber = 0;

            for (int i = 0; i < height*width; i++){
                if (i < width || i > height*width-width-1 || i%width==0 || (i+1)%width==0){
                    this->content.push_back('*');
                }else{
                    this->content.push_back(' ');
                }
            }
        }
        Room(int width, int height, int number, char content[]){
            this->height = height;
            this->width = width;
            this->number = number;
            this->upRoomNumber = 0;
            this->downRoomNumber = 0;
            this->leftRoomNumber = 0;
            this->rightRoomNumber = 0;

            for (int i = 0; i < height*width; i++){
                this->content.push_back(content[i]);
            }
        }
        Room(int width, int height, int number, int upRoomNumber, int downRoomNumber, int leftRoomNumber, int rightRoomNumber, char content[]){
            this->height = height;
            this->width = width;
            this->number = number;
            this->upRoomNumber = upRoomNumber;
            this->downRoomNumber = downRoomNumber;
            this->leftRoomNumber = leftRoomNumber;
            this->rightRoomNumber = rightRoomNumber;

            for (int i = 0; i < height*width; i++){
                this->content.push_back(content[i]);
            }
        }

        char getContent(int p){
            return content[p];
        }
        int getWidth(){
            return width;
        }
        int getHeight(){
            return height;
        }
        int getRoomSize(){
            return width*height;
        }
};

class World{
    private:
        vector<Room*> roomList;
    public:
        Room* getRoom(int n){
            for (int i = 0; i < roomList.size(); i++){
                if (roomList[i]->number == n){
                    return roomList[i];
                }
            }
            return 0;
        }

        void addRoom(Room *room){
            roomList.push_back(room);
        }
};

class Player{
    private:
        void goUpRoom(){
            int x = position % getCurrentRoom()->getWidth();
            currentRoomNumber = getCurrentRoom()->upRoomNumber;
            position = getCurrentRoom()->getWidth() * (getCurrentRoom()->getHeight() - 1) + x;
        }
        void goDownRoom(){
            int x = position % getCurrentRoom()->getWidth();
            currentRoomNumber = getCurrentRoom()->downRoomNumber;
            position = x;
        }
        void goLeftRoom(){
            int y = position / getCurrentRoom()->getWidth();
            currentRoomNumber = getCurrentRoom()->leftRoomNumber;
            position = getCurrentRoom()->getWidth() * y + getCurrentRoom()->getWidth() - 1;
        }
        void goRightRoom(){
            int y = position / getCurrentRoom()->getWidth();
            currentRoomNumber = getCurrentRoom()->rightRoomNumber;
            position = getCurrentRoom()->getWidth() * y;
        }

    public:
        int position;
        World *currentWorld;
        int currentRoomNumber;
        
        void move(char dir){
            int bias = 0;
            if (dir == 'w'){
                if (position / getCurrentRoom()->getWidth() == 0){
                    goUpRoom();
                    return;
                } 
                bias = -getCurrentRoom()->getWidth();
            } 
            if (dir == 's'){
                if (position / getCurrentRoom()->getWidth() == getCurrentRoom()->getHeight() - 1){
                    goDownRoom();
                    return;
                }
                bias = getCurrentRoom()->getWidth();
            } 
            if (dir == 'a'){
                if (position % getCurrentRoom()->getWidth() == 0){
                    goLeftRoom();
                    return;
                }
                bias = -1;
            }
            if (dir == 'd'){
                if (position % getCurrentRoom()->getWidth() == getCurrentRoom()->getWidth() - 1){
                    goRightRoom();
                    return;
                }
                bias = 1;
            }
            if (getCurrentRoom()->getContent(position + bias) == ' '){
                position += bias;
            }
        }

        Room* getCurrentRoom(){
            return currentWorld->getRoom(currentRoomNumber);
        }
};

void display(Player*);
void worldSetup(World*, Player*);

int main(){
    char input = '0';
    World *world1 = new World();
    Player *player = new Player();

    worldSetup(world1, player);

    while(input != 'z'){
        display(player);
        input = getch();
        player->move(input);
    }
}

void display(Player *player){
    system("cls");
    for (int i = 0; i < player->getCurrentRoom()->getRoomSize(); i++){
        if (i == player->position){
            cout << '@' << ' ';
        }else{
            cout << player->getCurrentRoom()->getContent(i) << ' ';
        }
        if (i%player->getCurrentRoom()->getWidth() == player->getCurrentRoom()->getWidth()-1) cout << '\n';
    }
}

void worldSetup(World *world, Player *player){
    char t1[] = {
        '*', '*', '*', '*', '*',
        '*', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', ' ',
        '*', ' ', ' ', ' ', '*',
        '*', '*', ' ', '*', '*',
    };
    Room *room1 = new Room(5, 5, 0, t1);
    room1->rightRoomNumber = 1;
    room1->downRoomNumber = 2;
    world->addRoom(room1);

    char t2[] = {
        '*', '*', '*', '*', '*', '*', '*',
        '*', '*', ' ', ' ', ' ', ' ', '*',
        ' ', ' ', ' ', ' ', ' ', ' ', '*',
        '*', '*', ' ', ' ', ' ', ' ', '*',
        '*', '*', ' ', '*', '*', '*', '*',
    };
    Room *room2 = new Room(7, 5, 1, t2);
    room2->rightRoomNumber = 0;
    room2->downRoomNumber = 3;
    world->addRoom(room2);

    char t3[] = {
        '*', '*', ' ', '*', '*', '*', '*',
        '*', '*', ' ', ' ', ' ', '*', '*',
        '*', '*', ' ', ' ', ' ', ' ', ' ',
        '*', '*', ' ', ' ', ' ', '*', '*',
        '*', '*', '*', '*', '*', '*', '*',
    };
    Room *room3 = new Room(7, 5, 2, t3);
    room3->upRoomNumber = 0;
    room3->rightRoomNumber = 3;
    world->addRoom(room3);

    char t4[] = {
        '*', '*', ' ', '*', '*',
        '*', ' ', ' ', ' ', '*',
        ' ', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', '*',
        '*', '*', '*', '*', '*',
    };
    Room *room4 = new Room(5, 7, 3, t4);
    room4->upRoomNumber = 1;
    room4->leftRoomNumber = 2;
    world->addRoom(room4);

    player->currentWorld = world;
    player->currentRoomNumber = 0;
    player->position = player->getCurrentRoom()->getWidth()+1;
}