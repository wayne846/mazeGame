#include <iostream>
#include <conio.h>
#include <vector>
#include <string>
#include <queue>
#include <list>

//目前沒甚麼用的標籤，只有觀察者模式會用到
#define TAG_WALL "wall"
#define TAG_AIR "air"
#define TAG_BILLBOARD "billboard"
#define TAG_LEVER "lever"
#define TAG_GATE "gate"

using namespace std;

class World;
class Room;
class Player;
class Subject;
class Observer;

//單例模式
class Message{
    private:
        Message(){
            message = "";
        }
        string message;
        static Message* instance;
    public:
        static Message* getInstance(){
            if (instance == nullptr){
                instance = new Message();
            }
            return instance;
        }

        void addMessage(string str){
            message = message + str + '\n';
        }
        void addMessage(char c){
            string s;
            s = c; 
            message = message + s + '\n';
        }
        void addMessage(int i){
            message = message + to_string(i) + '\n';
        }
        void clearMessage(){
            message.clear();
        }
        string getMessage(){
            return message;
        }
};
Message* Message::instance = nullptr;


//觀察者模式
class Subject{
    protected:
        string tag;
        int id;
    public:
        virtual void registerObserver(Observer *o) = 0;
        virtual void removeObserver(Observer *o) = 0;
        virtual void notifyObservers() = 0;

        string getTag(){
            return tag;
        }
        int getId(){
            return id;
        }
        void setId(int i){
            id = i;
        }
};
class Observer{
    protected:
        string tag;
        int id;
        Subject *subject;
    public:
        virtual void update() = 0;

        string getTag(){
            return tag;
        }
        int getId(){
            return id;
        }
        void setId(int i){
            id = i;
        }
        void setSubject(Subject *s){
            subject = s;
            subject->registerObserver(this);
        }
};

//用來初始化世界
class WorldSetter{
    private:
        static list<Subject*> subjectList;
        static list<Observer*> observerList;
        static Room* createRoom1();
        static Room* createRoom2();
        static Room* createRoom3();
        static Room* createRoom4();
        static void linkingTiles();

    public:
        static void worldSetup(World*, Player*);
        static void addSubject(Subject*);
        static void addObserver(Observer*);
};

//給所有種類的磚塊繼承用
class Tile{
    protected:
        char symbol;
        bool isBlock;
        bool isInteractive;
    
    public:
        virtual void interact() = 0;
        virtual void detected() = 0;

        bool getIsBlock(){
            return isBlock;
        }
        bool getIsInteractive(){
            return isInteractive;
        }
        char getSymbol(){
            return symbol;
        }
};

class Wall : public Tile{
    public:
        Wall(){
            this->symbol = '*';
            this->isBlock = true;
            this->isInteractive = false;
        }

        void interact(){}
        void detected(){}
};

class Air : public Tile{
    public:
        Air(){
            this->symbol = ' ';
            this->isBlock = false;
            this->isInteractive = false;
        }

        void interact(){}
        void detected(){}
};

//告示牌
class Billboard : public Tile{
    private:
        string message;
    public:
        Billboard(){
            this->symbol = '!';
            this->isBlock = true;
            this->isInteractive = true;
            this->message = "";
        }

        void interact(){
            Message::getInstance()->addMessage(message);
        }
        void detected(){
            Message::getInstance()->addMessage("press e to read '!'");
        }

        void setMessage(string m){
            message = m;
        }
};

//拉桿
class Lever : public Tile, public Subject{
    private:
        list<Observer*> observers;
    public:
        Lever(){
            this->symbol = '/';
            this->tag = TAG_LEVER;
            this->id = 0;
            this->isBlock = true;
            this->isInteractive = true;
        }

        void interact(){
            Message::getInstance()->addMessage("The door in somewhere open");
            notifyObservers();
            isInteractive = false;
        }
        void detected(){
            Message::getInstance()->addMessage("press e to use the lever('/')");
        }

        void registerObserver(Observer *o){
            observers.push_back(o);
        }
        void removeObserver(Observer *o){
            observers.remove(o);
        }
        void notifyObservers(){ //list不能在迭代過程中修改元素 "erase list iterator"，所以先丟進vector
            vector<Observer*> v;
            for(Observer *observer : observers){
                v.push_back(observer);
            }
            for(int i = 0; i < v.size(); i++){
                v[i]->update();
            }
        }
};

//閘門，由拉桿開啟
class Gate : public Tile, public Observer{
    public:
        Gate(){
            this->symbol = 'D';
            this->tag = TAG_GATE;
            this->id = 0;
            this->isBlock = true;
            this->isInteractive = false;
        }

        void interact(){
            symbol = ' ';
            isBlock = false;
            subject->removeObserver(this);
        }
        void detected(){}

        void update(){
            interact();
        }

};

//製造Tile，還可以修改Tile 簡單工廠
class TillFactory{
    private:
        queue<string> billboardMessageList; //Billboard的訊息 依序放入
        queue<int> leverIdList;
        queue<int> gateIdList;

    public:
        TillFactory(){

        }

        Tile* createTile(char a){
            if (a == '*') return new Wall();
            if (a == ' ') return new Air();
            if (a == '!'){
                Billboard *b = new Billboard();
                if (!billboardMessageList.empty()){
                    b->setMessage(billboardMessageList.front());
                    billboardMessageList.pop();
                }
                return b;
            }
            if (a == '/'){
                Lever *l = new Lever();
                if (!leverIdList.empty()){
                    l->setId(leverIdList.front());
                    leverIdList.pop();
                }
                WorldSetter::addSubject(l);
                return l;
            }
            if (a == 'D'){
                Gate *g = new Gate();
                if (!gateIdList.empty()){
                    g->setId(gateIdList.front());
                    gateIdList.pop();
                }
                WorldSetter::addObserver(g);
                return g;
            }
            return 0;
        }

        void addBillboardMessage(string s){
            billboardMessageList.push(s);
        }
        void addLeverId(int i){
            leverIdList.push(i);
        }
        void addGateId(int i){
            gateIdList.push(i);
        }
};

class Room{
    private:
        int width;
        int height;
        int number; //房間編號
        int upRoomNumber; //往上連通的房間編號
        int downRoomNumber; //往下連通的房間編號
        int leftRoomNumber; //往左連通的房間編號
        int rightRoomNumber; //往右連通的房間編號
        vector<Tile*> content; //存房間中的布局

        TillFactory *factory;

    public:
        Room(int width, int height, int number, TillFactory *factory){
            this->height = height;
            this->width = width;
            this->number = number;
            this->factory = factory;
            this->upRoomNumber = 0;
            this->downRoomNumber = 0;
            this->leftRoomNumber = 0;
            this->rightRoomNumber = 0;

            for (int i = 0; i < height*width; i++){
                if (i < width || i > height*width-width-1 || i%width==0 || (i+1)%width==0){
                    this->content.push_back(new Wall());
                }else{
                    this->content.push_back(0);
                }
            }
        }
        Room(int width, int height, int number, TillFactory *factory, char content[]){
            this->height = height;
            this->width = width;
            this->number = number;
            this->factory = factory;
            this->upRoomNumber = 0;
            this->downRoomNumber = 0;
            this->leftRoomNumber = 0;
            this->rightRoomNumber = 0;

            for (int i = 0; i < height*width; i++){
                
                this->content.push_back(factory->createTile(content[i]));
            }
        }
        Room(int width, int height, int number, int upRoomNumber, int downRoomNumber, int leftRoomNumber, int rightRoomNumber, TillFactory *factory, char content[]){
            this->height = height;
            this->width = width;
            this->number = number;
            this->factory = factory;
            this->upRoomNumber = upRoomNumber;
            this->downRoomNumber = downRoomNumber;
            this->leftRoomNumber = leftRoomNumber;
            this->rightRoomNumber = rightRoomNumber;

            for (int i = 0; i < height*width; i++){
                this->content.push_back(factory->createTile(content[i]));
            }
        }

        Tile* getContent(int p){
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
        int getNumber(){
            return number;
        }
        int getUpRoomNumber(){
            return upRoomNumber;
        }
        int getDownRoomNumber(){
            return downRoomNumber;
        }
        int getLeftRoomNumber(){
            return leftRoomNumber;
        }
        int getRightRoomNumber(){
            return rightRoomNumber;
        }
        void setUpRoomNumber(int n){
            upRoomNumber = n;
        }
        void setDownRoomNumber(int n){
            downRoomNumber = n;
        }
        void setLeftRoomNumber(int n){
            leftRoomNumber = n;
        }
        void setRightRoomNumber(int n){
            rightRoomNumber = n;
        }
};

class World{
    private:
        vector<Room*> roomList;
    public:
        Room* getRoom(int n){
            for (int i = 0; i < roomList.size(); i++){
                if (roomList[i]->getNumber() == n){
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
        char symbol;
        Tile *interactTile;

        //移到另一個房間，更改房間編號、玩家位置
        void goUpRoom(){
            int x = position % getCurrentRoom()->getWidth();
            currentRoomNumber = getCurrentRoom()->getUpRoomNumber();
            position = getCurrentRoom()->getWidth() * (getCurrentRoom()->getHeight() - 1) + x;
        }
        void goDownRoom(){
            int x = position % getCurrentRoom()->getWidth();
            currentRoomNumber = getCurrentRoom()->getDownRoomNumber();
            position = x;
        }
        void goLeftRoom(){
            int y = position / getCurrentRoom()->getWidth();
            currentRoomNumber = getCurrentRoom()->getLeftRoomNumber();
            position = getCurrentRoom()->getWidth() * y + getCurrentRoom()->getWidth() - 1;
        }
        void goRightRoom(){
            int y = position / getCurrentRoom()->getWidth();
            currentRoomNumber = getCurrentRoom()->getRightRoomNumber();
            position = getCurrentRoom()->getWidth() * y;
        }

        void detectAround(char dir){
            interactTile = 0;
            int width = getCurrentRoom()->getWidth();
            int size = getCurrentRoom()->getRoomSize();

            if (position >= width && getCurrentRoom()->getContent(position-width)->getIsInteractive()){
                interactTile = getCurrentRoom()->getContent(position-width);
                if (dir == 'w') return;
            }
            if (position < size-width  && getCurrentRoom()->getContent(position+width)->getIsInteractive()){
                interactTile = getCurrentRoom()->getContent(position+width);
                if (dir == 's') return;
            }
            if (position % width != 0 && getCurrentRoom()->getContent(position-1)->getIsInteractive()){
                interactTile = getCurrentRoom()->getContent(position-1);
                if (dir == 'a') return;
            }
            if (position % width != width-1 && getCurrentRoom()->getContent(position+1)->getIsInteractive()){
                interactTile = getCurrentRoom()->getContent(position+1);
                if (dir == 'd') return;
            }

            
        }

    public:
        int position;
        World *currentWorld;
        int currentRoomNumber;
        
        Player(){
            this->symbol = '@';
            this->interactTile = 0;
        }

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
            if (getCurrentRoom()->getContent(position + bias) == 0){
                return;
            }else if (getCurrentRoom()->getContent(position + bias)->getIsBlock() == false){
                position += bias;
            }
            
            detectAround(dir);
            if (interactTile != 0) interactTile->detected();
        }

        void interact(){
            if (interactTile == 0) return;
            interactTile->interact();
        }

        Room* getCurrentRoom(){
            return currentWorld->getRoom(currentRoomNumber);
        }
        char getSymbol(){
            return symbol;
        }
        
};

//類別間互相使用 所以要先宣告再定義
list<Subject*> WorldSetter::subjectList;
list<Observer*> WorldSetter::observerList;
Room* WorldSetter::createRoom1(){
    char t[] = {
        '*', '*', '!', '*', '*',
        '*', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', ' ',
        '*', ' ', ' ', ' ', '*',
        '*', '*', ' ', '*', '*',
    };
    TillFactory *factory = new TillFactory();
    factory->addBillboardMessage("Room 1");
    Room *room = new Room(5, 5, 0, factory, t);
    room->setRightRoomNumber(1);
    room->setDownRoomNumber(2);
    return room;
}
Room* WorldSetter::createRoom2(){
    char t[] = {
        '*', '*', '!', '*', '*', '!', '*',
        '*', '*', ' ', ' ', ' ', ' ', '*',
        ' ', ' ', ' ', ' ', ' ', ' ', '/',
        '*', '*', ' ', ' ', ' ', ' ', '*',
        '*', '*', ' ', '*', '*', '*', '*',
    };
    TillFactory *factory = new TillFactory();
    factory->addBillboardMessage("Room 2");
    factory->addBillboardMessage("Go to Study!!!");
    Room *room = new Room(7, 5, 1, factory, t);
    room->setRightRoomNumber(0);
    room->setDownRoomNumber(3);
    return room;
}
Room* WorldSetter::createRoom3(){
    char t[] = {
        '*', '*', ' ', '*', '*', '*', '*',
        '*', '*', ' ', ' ', ' ', '*', '*',
        '*', '*', ' ', ' ', ' ', ' ', 'D',
        '*', '*', ' ', ' ', ' ', '*', '*',
        '*', '*', '*', '!', '*', '*', '*',
    };
    TillFactory *factory = new TillFactory();
    factory->addBillboardMessage("Room 3");
    Room *room = new Room(7, 5, 2, factory, t);
    room->setUpRoomNumber(0);
    room->setRightRoomNumber(3);
    return room;
}
Room* WorldSetter::createRoom4(){
    char t[] = {
        '*', '*', ' ', '*', '*',
        '*', ' ', ' ', ' ', '*',
        'D', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', '!',
        '*', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', '*',
        '*', '*', '*', '*', '*',
    };
    TillFactory *factory = new TillFactory();
    factory->addBillboardMessage("Room 4");
    Room *room = new Room(5, 7, 3, factory, t);
    room->setUpRoomNumber(1);
    room->setLeftRoomNumber(2);
    return room;
}
void WorldSetter::linkingTiles(){
    for (Subject *subject : subjectList){
        for (Observer *observer : observerList){
            if (subject->getTag() == TAG_LEVER && observer->getTag() == TAG_GATE && subject->getId() == observer->getId()){
                observer->setSubject(subject);
            }
        }
    }
}
void WorldSetter::worldSetup(World* world, Player* player){
    world->addRoom(createRoom1());
    world->addRoom(createRoom2());
    world->addRoom(createRoom3());
    world->addRoom(createRoom4());

    linkingTiles();

    player->currentWorld = world;
    player->currentRoomNumber = 0;
    player->position = player->getCurrentRoom()->getWidth()+1;
}
void WorldSetter::addSubject(Subject* s){
    subjectList.push_back(s);
}
void WorldSetter::addObserver(Observer* o){
    observerList.push_back(o);
}


void display(Player*);

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

void display(Player *player){
    system("cls");
    for (int i = 0; i < player->getCurrentRoom()->getRoomSize(); i++){
        if (i == player->position){
            cout << player->getSymbol() << ' ';
        }else if (player->getCurrentRoom()->getContent(i) == 0){
            cout << ' ' << ' ';
        }else{
            cout << player->getCurrentRoom()->getContent(i)->getSymbol() << ' ';
        }
        if (i%player->getCurrentRoom()->getWidth() == player->getCurrentRoom()->getWidth()-1) cout << '\n';
    }

    Message *message = Message::getInstance();
    cout << message->getMessage() << '\n';
    message->clearMessage();
}
