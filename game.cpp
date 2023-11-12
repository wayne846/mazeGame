#include "game.h"
#include <iostream>
#include <conio.h>

using namespace std;

//Message
Message::Message(){
    message = "";
}
Message* Message::getInstance(){
    if (instance == nullptr){
        instance = new Message();
    }
    return instance;
}
void Message::addMessage(string str){
    message = message + str + '\n';
}
void Message::addMessage(char c){
    string s;
    s = c; 
    message = message + s + '\n';
}
void Message::addMessage(int i){
    message = message + to_string(i) + '\n';
}
void Message::clearMessage(){
    message.clear();
}
string Message::getMessage(){
    return message;
}
Message* Message::instance = nullptr;

//Subject
string Subject::getTag(){
    return tag;
}
int Subject::getId(){
    return id;
}
void Subject::setId(int i){
    id = i;
}

//Observer
string Observer::getTag(){
    return tag;
}
int Observer::getId(){
    return id;
}
void Observer::setId(int i){
    id = i;
}
void Observer::setSubject(Subject *s){
    subject = s;
    subject->registerObserver(this);
}

//Tile
bool Tile::getIsBlock(){
    return isBlock;
}
bool Tile::getIsInteractive(){
    return isInteractive;
}
char Tile::getSymbol(){
    return symbol;
}

//Wall
Wall::Wall(){
    this->symbol = '*';
    this->isBlock = true;
    this->isInteractive = false;
}
void Wall::interact(){}
void Wall::detected(){}

//Air
Air::Air(){
    this->symbol = ' ';
    this->isBlock = false;
    this->isInteractive = false;
}
void Air::interact(){}
void Air::detected(){}

//Billboard
Billboard::Billboard(){
    this->symbol = '!';
    this->isBlock = true;
    this->isInteractive = true;
    this->message = "";
}
void Billboard::interact(){
    Message::getInstance()->addMessage(message);
}
void Billboard::detected(){
    Message::getInstance()->addMessage("press e to read '!'");
}
void Billboard::setMessage(string m){
    message = m;
}

//Lever
Lever::Lever(){
    this->symbol = '/';
    this->tag = TAG_LEVER;
    this->id = 0;
    this->isBlock = true;
    this->isInteractive = true;
}
void Lever::interact(){
    Message::getInstance()->addMessage("The door in somewhere open");
    notifyObservers();
    isInteractive = false;
    symbol = '\\';
}
void Lever::detected(){
    Message::getInstance()->addMessage("press e to use the lever('/')");
}
void Lever::registerObserver(Observer *o){
    observers.push_back(o);
}
void Lever::removeObserver(Observer *o){
    observers.remove(o);
}
void Lever::notifyObservers(){  //list不能在迭代過程中修改元素 "erase list iterator"，所以先丟進vector
    vector<Observer*> v;
    for(Observer *observer : observers){
        v.push_back(observer);
    }
    for(int i = 0; i < v.size(); i++){
        v[i]->update();
    }
}

//Gate
Gate::Gate(){
    this->symbol = 'D';
    this->tag = TAG_GATE;
    this->id = 0;
    this->isBlock = true;
    this->isInteractive = false;
}
void Gate::interact(){
    symbol = ' ';
    isBlock = false;
    subject->removeObserver(this);
}
void Gate::detected(){}
void Gate::update(){
    interact();
}

//Rock
Rock::Rock(){
    this->symbol = 'R';
    this->isBlock = true;
    this->isInteractive = false;
}
void Rock::interact(){}
void Rock::detected(){}
bool Rock::getIsBlock(){
    char c = GameController::getPlayerInput();
    isBlock = tryMove();
    return isBlock;
}
bool Rock

//TillFactory
TillFactory::TillFactory(){}
Tile* TillFactory::createTile(char a){
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
        GameController::addSubject(l);
        return l;
    }
    if (a == 'D'){
        Gate *g = new Gate();
        if (!gateIdList.empty()){
            g->setId(gateIdList.front());
            gateIdList.pop();
        }
        GameController::addObserver(g);
        return g;
    }
    return 0;
}
void TillFactory::addBillboardMessage(string s){
    billboardMessageList.push(s);
}
void TillFactory::addLeverId(int i){
    leverIdList.push(i);
}
void TillFactory::addGateId(int i){
    gateIdList.push(i);
}

//Room
Room::Room(int width, int height, int number, TillFactory *factory){
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
Room::Room(int width, int height, int number, TillFactory *factory, char content[]){
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
Room::Room(int width, int height, int number, int upRoomNumber, int downRoomNumber, int leftRoomNumber, int rightRoomNumber, TillFactory *factory, char content[]){
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
Tile* Room::getContent(int p){
    return content[p];
}
int Room::getWidth(){
    return width;
}
int Room::getHeight(){
    return height;
}
int Room::getRoomSize(){
    return width*height;
}
int Room::getNumber(){
    return number;
}
int Room::getUpRoomNumber(){
    return upRoomNumber;
}
int Room::getDownRoomNumber(){
    return downRoomNumber;
}
int Room::getLeftRoomNumber(){
    return leftRoomNumber;
}
int Room::getRightRoomNumber(){
    return rightRoomNumber;
}
void Room::setUpRoomNumber(int n){
    upRoomNumber = n;
}
void Room::setDownRoomNumber(int n){
    downRoomNumber = n;
}
void Room::setLeftRoomNumber(int n){
    leftRoomNumber = n;
}
void Room::setRightRoomNumber(int n){
    rightRoomNumber = n;
}
int Room::upPosition(int p){
    if(p - width < 0) return -1;
    return p-width;
}
int Room::downPosition(int p){
    if(p + width >= width*height) return -1;
    return p+width;
}
int Room::leftPosition(int p){
    if(p % width == 0) return -1;
    return p-1;
}
int Room::rightPosition(int p){
    if((p+1) % width == 0) return -1;
    return p+1;
}

//World
Room* World::getRoom(int n){
    for (int i = 0; i < roomList.size(); i++){
        if (roomList[i]->getNumber() == n){
            return roomList[i];
        }
    }
    return 0;
}
void World::addRoom(Room *room){
    roomList.push_back(room);
}

//Player
Player::Player(){
    this->symbol = '@';
    this->interactTile = 0;
}
void Player::goUpRoom(){
    int x = position % getCurrentRoom()->getWidth();
    currentRoomNumber = getCurrentRoom()->getUpRoomNumber();
    position = getCurrentRoom()->getWidth() * (getCurrentRoom()->getHeight() - 1) + x;
}
void Player::goDownRoom(){
    int x = position % getCurrentRoom()->getWidth();
    currentRoomNumber = getCurrentRoom()->getDownRoomNumber();
    position = x;
}
void Player::goLeftRoom(){
    int y = position / getCurrentRoom()->getWidth();
    currentRoomNumber = getCurrentRoom()->getLeftRoomNumber();
    position = getCurrentRoom()->getWidth() * y + getCurrentRoom()->getWidth() - 1;
}
void Player::goRightRoom(){
    int y = position / getCurrentRoom()->getWidth();
    currentRoomNumber = getCurrentRoom()->getRightRoomNumber();
    position = getCurrentRoom()->getWidth() * y;
}
void Player::detectAround(char dir){
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
void Player::move(char dir){
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
void Player::interact(){
    if (interactTile == 0) return;
    interactTile->interact();
}
Room* Player::getCurrentRoom(){
    return currentWorld->getRoom(currentRoomNumber);
}
char Player::getSymbol(){
    return symbol;
}
int Player::position;

//GameController
list<Subject*> GameController::subjectList;
list<Observer*> GameController::observerList;
Room* GameController::createRoom1(){
    char t[] = {
        '*', '*', '!', '*', '*', '*', '*',
        '*', ' ', ' ', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', ' ', ' ', '/',
        '*', ' ', ' ', ' ', ' ', ' ', 'D',
        '*', ' ', ' ', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', ' ', ' ', '*',
        '*', '*', '*', '*', '*', '*', '*'
    };
    TillFactory *factory = new TillFactory();
    factory->addBillboardMessage("Room 1");
    Room *room = new Room(7, 8, 0, factory, t);
    room->setRightRoomNumber(1);
    room->setDownRoomNumber(2);
    return room;
}
Room* GameController::createRoom2(){
    char t[] = {
        '*', '*', '!', '*', '*', '*', '*',
        '*', ' ', ' ', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', ' ', ' ', '*',
        'D', ' ', ' ', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', ' ', ' ', '*',
        '*', ' ', ' ', ' ', ' ', ' ', '*',
        '*', '*', '*', '*', '*', '*', '*'
    };
    TillFactory *factory = new TillFactory();
    factory->addBillboardMessage("Room 2");
    Room *room = new Room(7, 8, 1, factory, t);
    room->setRightRoomNumber(0);
    room->setDownRoomNumber(3);
    return room;
}
Room* GameController::createRoom3(){
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
Room* GameController::createRoom4(){
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
void GameController::linkingTiles(){
    for (Subject *subject : subjectList){
        for (Observer *observer : observerList){
            if (subject->getTag() == TAG_LEVER && observer->getTag() == TAG_GATE && subject->getId() == observer->getId()){
                observer->setSubject(subject);
            }
        }
    }
}
void GameController::worldSetup(World* world, Player* player){
    world->addRoom(createRoom1());
    world->addRoom(createRoom2());

    linkingTiles();

    player->currentWorld = world;
    player->currentRoomNumber = 0;
    player->position = player->getCurrentRoom()->getWidth()+1;
}
void GameController::addSubject(Subject* s){
    subjectList.push_back(s);
}
void GameController::addObserver(Observer* o){
    observerList.push_back(o);
}
char GameController::input(){
    playerInput = getch();
    return playerInput;
}
char GameController::getPlayerInput(){
    return playerInput;
}
char GameController::playerInput = ' ';

//display
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
    message->addMessage(Player::position);
    cout << message->getMessage() << '\n';
    message->clearMessage();
}