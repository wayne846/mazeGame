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

class Subject;
class Observer;

//單例模式
//存放所有在遊戲畫面下方顯示的訊息
class Message{
    private:
        Message();
        string message;
        static Message* instance;
    public:
        static Message* getInstance();

        void addMessage(string);
        void addMessage(char);
        void addMessage(int);
        void clearMessage();
        string getMessage();
};

//觀察者模式
class Subject{
    protected:
        string tag; //區別不同種類的磚塊
        int id; //同個id會配在一起
    public:
        virtual void registerObserver(Observer *o) = 0;
        virtual void removeObserver(Observer *o) = 0;
        virtual void notifyObservers() = 0;

        string getTag();
        int getId();
        void setId(int);
};
class Observer{
    protected:
        string tag; //區別不同種類的磚塊
        int id; //同個id會配在一起
        Subject *subject;
    public:
        virtual void update() = 0;

        string getTag();
        int getId();
        void setId(int);
        void setSubject(Subject*);
};

//給所有種類的磚塊繼承用
class Tile{
    protected:
        char symbol; //顯示出來的符號
        bool isBlock; //是否可阻擋玩家
        bool isInteractive; //是否可被互動
    
    public:
        virtual void interact() = 0;
        virtual void detected() = 0;

        bool getIsBlock();
        bool getIsInteractive();
        char getSymbol();
};

//牆壁，阻擋玩家用
class Wall : public Tile{
    public:
        Wall();

        void interact();
        void detected();
};

//空氣
class Air : public Tile{
    public:
        Air();

        void interact();
        void detected();
};

//告示牌，可顯示文字
class Billboard : public Tile{
    private:
        string message;
    public:
        Billboard();

        void interact();
        void detected();
        void setMessage(string);
};

//拉桿
class Lever : public Tile, public Subject{
    private:
        list<Observer*> observers;
    public:
        Lever();

        void interact();
        void detected();
        void registerObserver(Observer*);
        void removeObserver(Observer*);
        void notifyObservers();
};

//閘門，由拉桿開啟，開啟後將自己移除拉桿的觀察者名單
class Gate : public Tile, public Observer{
    public:
        Gate();

        void interact();
        void detected();
        void update();

};

//製造Tile，還可以修改Tile 簡單工廠
class TillFactory{
    private:
        queue<string> billboardMessageList; //告示牌的訊息 依序放入
        queue<int> leverIdList; //拉桿的id 依序放入
        queue<int> gateIdList; //閘門的id 依序放入

    public:
        TillFactory();

        Tile* createTile(char); //傳入字元，回傳對應的磚塊，特別的磚塊會用對應的list修改參數
        void addBillboardMessage(string);
        void addLeverId(int);
        void addGateId(int);
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
        Room(int width, int height, int number, TillFactory *factory); //空房間，四周是牆壁
        Room(int width, int height, int number, TillFactory *factory, char content[]);
        Room(int width, int height, int number, int upRoomNumber, int downRoomNumber, int leftRoomNumber, int rightRoomNumber, TillFactory *factory, char content[]);

        Tile* getContent(int);
        int getWidth();
        int getHeight();
        int getRoomSize();
        int getNumber();
        int getUpRoomNumber();
        int getDownRoomNumber();
        int getLeftRoomNumber();
        int getRightRoomNumber();
        void setUpRoomNumber(int);
        void setDownRoomNumber(int);
        void setLeftRoomNumber(int);
        void setRightRoomNumber(int);
};

//世界由多個房間組成
class World{
    private:
        vector<Room*> roomList;
    public:
        Room* getRoom(int);

        void addRoom(Room*);
};

//玩家
class Player{
    private:
        char symbol;
        Tile *interactTile; //正在互動的磚塊

        //移到另一個房間，更改房間編號、玩家位置
        void goUpRoom();
        void goDownRoom();
        void goLeftRoom();
        void goRightRoom();

        void detectAround(char dir); //檢測周圍可互動的磚塊，玩家面向的方向優先

    public:
        int position;
        World *currentWorld;
        int currentRoomNumber;
        
        Player();

        void move(char dir); //根據傳入的wasd來做上下左右的移動，並檢測周圍可互動的磚塊
        void interact(); //與磚塊互動

        Room* getCurrentRoom();
        char getSymbol();
        
};

//用來初始化世界，感覺是個笨方法
class WorldSetter{
    private:
        static list<Subject*> subjectList;
        static list<Observer*> observerList;
        static Room* createRoom1();
        static Room* createRoom2();
        static Room* createRoom3();
        static Room* createRoom4();
        static void linkingTiles(); //將觀察者模式的物件連結起來

    public:
        static void worldSetup(World*, Player*);
        static void addSubject(Subject*);
        static void addObserver(Observer*);
};

//顯示畫面
void display(Player*);
