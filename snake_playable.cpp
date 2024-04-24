#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <windows.h>
#include <thread>
#include <chrono>
#include<unistd.h> 
#include <conio.h>
#include <queue>

using namespace std;

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

#define FL(n,i) for(int i = 0; i < n; i++)
#define MP(A,B) make_pair(A,B)

bool gameover = false;
int score = 0;
int best_score = 0;
vector<vector<string>> mp(52,vector<string>(52));
queue<pair<int,int>> snake;
enum direction {QUIT, UP, DOWN, LEFT, RIGHT, DASH};
direction dir;
direction prevdir;
pair<int,int> apple;
pair<int,int> rock;
pair<int,int> head;
pair<int,int> tail;


HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

void ClearScreen(int x, int y)
{   
COORD cursorPosition;   cursorPosition.X = x;   cursorPosition.Y = y;
SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

void setcursor(bool visible, DWORD size)  
{
    if(size == 0)
    {
        size = 20;
    }
    CONSOLE_CURSOR_INFO lpCursor;   
    lpCursor.bVisible = visible;
    lpCursor.dwSize = size;
    SetConsoleCursorInfo(console,&lpCursor);
}

void setconsolecolor(int textColor, int bgColor) 
{
SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (textColor + 
(bgColor * 16)));
}

void setup(){
    setcursor(0,0);
    srand(time(0)); 
    queue<pair<int,int>>().swap(snake);
    dir = UP;
     for(int i = 0; i < 52; i++){
        for(int j = 0; j < 52; j++){
            if(j== 0 || j == 51 || i == 0|| i == 51) mp[i][j] = "wall";
            else mp[i][j] = "empty";
        }
    }
    apple.first = rand()%51 + 1;
    apple.second = rand()%51 + 1;
    mp[apple.first][apple.second] = "apple";
    for(int i = 40; i > 37; i--){
        snake.push(make_pair(i,25));
        mp[snake.back().first][snake.back().second] = "snake";
    }
    head = MP(38, 25);
    tail = MP(40, 25);
    return;
 }
void print(){
    ClearScreen(0,0);
    int count = 0;
    FL(52, x){
        FL(52, y){
            string p = mp[x][y];
            if(p == "wall"){ 
                cout.width(2);
                setconsolecolor(0,9);
                cout << " ";
                
                } 
            else if(p == "empty"){
                cout.width(2);
                setconsolecolor(1,0);
                cout << " ";
                count++;
                
            }
            else if(p == "snake"){
                cout.width(2);
                                if(x == head.first && y == head.second) setconsolecolor(4,7);
                else if(x == tail.first && y == tail.second) setconsolecolor(4, 8);
                else setconsolecolor(4,6);
                cout << " ";

            }
            else if(p == "apple"){
                cout.width(2);
                setconsolecolor(0,4);
                cout << " ";
                }
            else if(p == "rock"){
                cout.width(2);
                setconsolecolor(2,1);
                cout << " ";
            }
        }
        cout << endl;
    }
    return;
}

void input(){
    char key;
    prevdir = dir;
    if(kbhit()){
        key = getch();
        switch(key){
            case 'a':
                if(prevdir != RIGHT){ 
                    dir= LEFT;
                }
                break;
            case 's':
                if(prevdir != UP){
                    dir= DOWN;
                }
                break;
            case 'w':
                if(prevdir != DOWN){
                    dir= UP;
                }
                break;
            case 'd':
                if(prevdir != LEFT){
                    dir= RIGHT;
                }
                break;
            case 'q':
                dir= QUIT;
                break;
            case ' ':
                dir = DASH;
                break;
            default:
                break;
        }
    }
    return;
}

bool badbadrock(int y, int x){
    if(mp[y+1][x] == "snake" || mp[y][x+1] == "snake" 
    || mp[y][x-1] == "snake" || mp[y-1][x] == "snake") return true;
    return false;
}
void move(int y, int x){
    if(mp[y][x] == "wall" || mp[y][x] == "snake" 
    || mp[y][x] == "rock" || y < 0 || y > 50 || x < 0 || x > 50){
        gameover = true;
        return;
    }
    else{
        ClearScreen(head.second*2, head.first);
        cout.width(2);
        setconsolecolor(4,6);
        cout << " ";
        snake.push(make_pair(y,x));
        if(mp[y][x] == "apple"){
            score++;
            mp[y][x] = "snake";
            head = MP(y,x);
            do{
                apple.first = rand()%50 + 1;
                apple.second = rand()%50 + 1;
            }while(mp[apple.first][apple.second] != "empty");
            mp[apple.first][apple.second] = "apple";
            ClearScreen(apple.second*2, apple.first);
            cout.width(2);
            setconsolecolor(0,4);
            cout << " ";
            for(int i = 0; i < 5; i++){
                do{
                 rock.first = rand()%50 + 1;
                 rock.second = rand()%50 + 1;
                }while(mp[rock.first][rock.second] != "empty"  
                || badbadrock(rock.first,rock.second));
                 mp[rock.first][rock.second] = "rock";
                cout.width(2);
                setconsolecolor(2,1);
                ClearScreen(rock.second*2, rock.first);
                cout << " ";
            }
        }
        else{
            mp[snake.front().first][snake.front().second] = "empty";
            snake.pop();
            ClearScreen(tail.second*2, tail.first);
            cout.width(2);
            setconsolecolor(1,0);
            cout << " ";
            tail = MP(snake.front().first, snake.front().second);
            mp[y][x] = "snake";
            head = MP(y,x);
            ClearScreen(tail.second*2, tail.first);
            cout.width(2);
            setconsolecolor(4,8);
            cout << " ";
        }
        ClearScreen(x*2, y);
        cout.width(2);
        setconsolecolor(4,7);
        cout << " ";
        return;
    }
}
void logic(){
    int y = snake.back().first;
    int x = snake.back().second;
    switch(dir){
        case RIGHT:
            move(y, x+1);
            break;
        case UP:
            move(y-1, x);
            break;
        case DOWN:
            move(y+1, x);
            break;
        case LEFT:
            move(y, x-1);
            break;
        case QUIT:
            gameover = true;
            break;
        case DASH:
            if(prevdir == LEFT){
                move(y, x-4);
                dir = LEFT;
                break;
            }
            if(prevdir == RIGHT){
                move(y, x+4);
                dir = RIGHT;
                break;
            } 
            if(prevdir == DOWN){
                move(y+4, x);
                dir = DOWN;
                break;
            }
            if(prevdir == UP){
                move(y-4,x);
                dir = UP;
                break;
            }
            

        default: 
            break;
    }
    return;
}
int main(){
    string start = "NO";
    do{
        cin >> start;
    }while(start != "start");
    restart_point: cout << "restart";
    gameover = false;
    score = 0;
    setup();
    print();
    while(!gameover){
        input();
        logic();
        Sleep(100);
    }
    setconsolecolor(15,0);
    system("CLS");
    best_score = max(best_score, score);
    cout << "gameover!\nscore: " << score;
    cout << "\nbest score: " << best_score;
    cout << "\nenter restart to restart\n";
    string choice;
    cin >> choice;
    if(choice == "restart") goto restart_point;
    setcursor(1,10);
    return 0;
}