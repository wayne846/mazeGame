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

        virtual bool getIsBlock() = 0;
        virtual bool getIsInteractive() = 0;
        char getSymbol();
};

//牆壁，阻擋玩家用
class Wall : public Tile{
    public:
        Wall();

        void interact();
        void detected();

        bool getIsBlock();
        bool getIsInteractive();
};

//空氣
class Air : public Tile{
    public:
        Air();

        void interact();
        void detected();

        bool getIsBlock();
        bool getIsInteractive();
};

//告示牌，可顯示文字
class Billboard : public Tile{
    private:
        string message;
    public:
        Billboard();

        void interact();
        void detected();

        bool getIsBlock();
        bool getIsInteractive();

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

        bool getIsBlock();
        bool getIsInteractive();

        void registerObserver(Observer*);
        void removeObserver(Observer*);
        void notifyObservers();
};

//閘門，由拉桿開啟
class Gate : public Tile, public Observer{
    public:
        Gate();

        void interact(); //開啟後將自己移除拉桿的觀察者名單
        void detected();

        bool getIsBlock();
        bool getIsInteractive();

        void update();

};

//石頭，可以被推，不能一次推多顆石頭
class Rock : public Tile{
    public:
        Rock();

        void interact();
        void detected();

        bool getIsBlock();
        bool getIsInteractive();

        //移動石頭，可以就回傳true
        bool tryMove(char);
};

//簡單工廠
//製造Tile，還可以修改Tile
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
        void setContent(int, Tile*);
        
        //傳入wasd和位置，回傳其對應位置，超出範圍就回傳-1
        int biasPosition(char, int);
        int upPosition(int);
        int downPosition(int);
        int leftPosition(int);
        int rightPosition(int);

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
        static int position;
        World *currentWorld;
        int currentRoomNumber;
        
        Player();

        void move(char dir); //根據傳入的wasd來做上下左右的移動，並檢測周圍可互動的磚塊
        void interact(); //與磚塊互動

        Room* getCurrentRoom();
        char getSymbol();
        
};

//用來初始化世界，感覺是個笨方法
//可以在全域範圍接觸Player
class GameController{
    private:
        static list<Subject*> subjectList;
        static list<Observer*> observerList;
        static Room* createRoom1();
        static Room* createRoom2();
        static Room* createRoom3();
        static Room* createRoom4();
        static void linkingTiles(); //將觀察者模式的物件連結起來

        static char playerInput;
        static Player* player;

    public:
        static void worldSetup(World*, Player*);
        static void addSubject(Subject*);
        static void addObserver(Observer*);

        static char input();

        static void setPlayer(Player*);
        static int getPlayerPosition();
        static Room* getPlayerCurrentRoom();

        static char getPlayerInput();

        //顯示畫面
        static void display();
};


