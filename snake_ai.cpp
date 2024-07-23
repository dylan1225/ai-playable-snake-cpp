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
#include <unistd.h>

using namespace std;

bool gameover = false;
int score = 0;
int best_score = 0;
vector<vector<string>> mp(52,vector<string>(52));
queue<pair<int,int>> snake;
enum direction {UP, DOWN, LEFT, RIGHT};
direction dir;
direction prevdir;
pair<int,int> apple;
pair<int,int> rock;
pair<int,int> head;
pair<int,int> tail;
char delay = 'f';

typedef pair<int,int> pi; 
#define MP(A,B) make_pair(A,B)
#define PB(T) push_back(T)
#define vi vector<int> 
#define pi pair<int,int>

struct status{
    bool go_up;
    bool go_down;
    bool go_left;
    bool go_right;
};
vector<vector<int>> cycle_path(52, vector<int>(52, -1));
vector<vector<bool>> node_graph(27, vector<bool>(27));
vector<vector<status>> maze_graph(52, vector<status> (52));

void populate_maze(){
    for(int x = 1; x <= 50; x++){
        for(int y = 1; y <= 50; y++){
            if(x%2 == 1 && y%2 == 1){
                maze_graph[x][y].go_down = true;
                maze_graph[x][y].go_right = true;
            }
            if(x%2 == 1 && y%2 == 0){
                maze_graph[x][y].go_left = true;
                maze_graph[x][y].go_down = true;
            }
            if(x%2 == 0 && y%2 == 0){
                maze_graph[x][y].go_up = true;
                maze_graph[x][y].go_left = true;
            }
            if(x%2 == 0 && y%2 == 1){
                maze_graph[x][y].go_up = true;
                maze_graph[x][y].go_right = true;
            }
        }
    }
    return;
}

void recontruct_maze(){
    for(int y = 0; y <= 26; y++){
        for(int x = 0; x <= 26; x++){
            if(x == 0 || y == 0 || x == 26 || y == 26){
                node_graph[y][x] = true;
            }
        }
    }
    int x = rand()%25+1;
    int y = rand()%25+1;
    vector<pair<int,int>> av;
    av.push_back(MP(y,x));
    while(!av.empty()){
        int pos = rand()%av.size();
        int y = av[pos].first;
        int x = av[pos].second;
        av.erase(pos + av.begin());

        //connect left
        if(!node_graph[y][x-1]){
            pi t_l = MP(y*2 - 1, (x-1)*2);
            pi t_r = MP(y*2 - 1, x*2 - 1);
            pi b_l = MP(y*2, (x-1)*2);
            pi b_r = MP(y*2, x*2 - 1);
            maze_graph[t_l.first][t_l.second].go_down = false;
            maze_graph[t_l.first][t_l.second].go_right = true;
            maze_graph[b_l.first][b_l.second].go_up = false;
            maze_graph[b_l.first][b_l.second].go_right = true;
            maze_graph[b_r.first][b_r.second].go_up = false;
            maze_graph[b_r.first][b_r.second].go_left = true;
            maze_graph[t_r.first][t_r.second].go_down = false;
            maze_graph[t_r.first][t_r.second].go_left = true;
            av.push_back(MP(y,x-1));
            node_graph[y][x-1] = true;
        }
        //connect right
        if(!node_graph[y][x+1]){
            pi t_l = MP(y*2 - 1, x*2);
            pi t_r = MP(y*2 - 1, x*2 + 1);
            pi b_l = MP(y*2, x*2);
            pi b_r = MP(y*2, x*2 + 1);
            maze_graph[t_l.first][t_l.second].go_down = false;
            maze_graph[t_l.first][t_l.second].go_right = true;
            maze_graph[b_l.first][b_l.second].go_up = false;
            maze_graph[b_l.first][b_l.second].go_right = true;
            maze_graph[b_r.first][b_r.second].go_up = false;
            maze_graph[b_r.first][b_r.second].go_left = true;
            maze_graph[t_r.first][t_r.second].go_down = false;
            maze_graph[t_r.first][t_r.second].go_left = true;
            av.push_back(MP(y,x+1));
            node_graph[y][x+1] = true;
        }
        //connect down
        if(!node_graph[y+1][x]){
            pi t_l = MP(y*2, x*2-1);
            pi t_r = MP(y*2, x*2);
            pi b_l = MP(y*2+1, x*2 - 1);
            pi b_r = MP(y*2+1, x*2);
            maze_graph[t_l.first][t_l.second].go_right = false;
            maze_graph[t_l.first][t_l.second].go_down = true;
            maze_graph[b_l.first][b_l.second].go_right = false;
            maze_graph[b_l.first][b_l.second].go_up = true;
            maze_graph[b_r.first][b_r.second].go_left = false;
            maze_graph[b_r.first][b_r.second].go_up = true;
            maze_graph[t_r.first][t_r.second].go_left = false;
            maze_graph[t_r.first][t_r.second].go_down = true;
            av.push_back(MP(y+1,x));
            node_graph[y+1][x] = true;
        }
        //connect up
        if(!node_graph[y-1][x]){
            pi t_l = MP(y*2 - 2, x*2-1);
            pi t_r = MP(y*2 - 2, x*2);
            pi b_l = MP(y*2 - 1, x*2-1);
            pi b_r = MP(y*2 - 1, x*2);
            maze_graph[t_l.first][t_l.second].go_right = false;
            maze_graph[t_l.first][t_l.second].go_down = true;
            maze_graph[b_l.first][b_l.second].go_right = false;
            maze_graph[b_l.first][b_l.second].go_up = true;
            maze_graph[b_r.first][b_r.second].go_left = false;
            maze_graph[b_r.first][b_r.second].go_up = true;
            maze_graph[t_r.first][t_r.second].go_left = false;
            maze_graph[t_r.first][t_r.second].go_down = true;
            av.push_back(MP(y-1,x));
            node_graph[y-1][x] = true;
        }
    }
    return;
}

void traverse_maze(){
    int y = 36;
    int x = 25;
    int index = 1;
    while(1){
        cycle_path[y][x] = index;
        if(maze_graph[y][x].go_up && cycle_path[y-1][x] == -1){
            y--;
        }
        else if(maze_graph[y][x].go_left && cycle_path[y][x-1] == -1){
            x--;
        }
        else if(maze_graph[y][x].go_right && cycle_path[y][x+1] == -1){
            x++;
        }
        else if(maze_graph[y][x].go_down && cycle_path[y+1][x] == -1){
            y++;
        }
        else break;
        index++;
    }
    return;
}
void create_hamiltonian_cycle(){
    srand(time(0));
    // cout << "done" << endl;
    populate_maze();
    // cout << "done" << endl;
    recontruct_maze();
    // cout << "done" << endl;
    traverse_maze();
    // cout << "done" << endl;
    return;
}

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
    queue<pair<int,int>>().swap(snake);
    dir = UP;
    prevdir = UP;
     for(int i = 0; i < 52; i++){
        for(int j = 0; j < 52; j++){
            if(j== 0 || j == 51 || i == 0|| i == 51) mp[i][j] = "wall";
            else mp[i][j] = "empty";
        }
    }
    apple.first = rand()%51 + 1;
    apple.second = rand()%51 + 1;
    mp[apple.first][apple.second] = "apple";
    for(int i = 40; i > 35; i--){
        snake.push(make_pair(i,25));
        mp[snake.back().first][snake.back().second] = "snake";
    }
    head = MP(36, 25);
    tail = MP(40, 25);
    return;
 }
void print(){
    ClearScreen(0,0);
    int count = 0;
    for(int x = 0; x < 52; x++){
        for(int y = 0; y < 52; y++){
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
        }
        cout << endl;
    }
    return;
}

int reindex(int a, int b){
    if(a > b) return a - b + 1;
    else return ((2500) + (a-(b-1)));
}

bool auto_next(int y, int x){
    if(cycle_path[head.first][head.second] == cycle_path[y][x] - 1 && (mp[y][x] == "empty" || mp[y][x] == "apple" || (y == tail.first && x == tail.second))) return true;
    else return false;
}

bool auto_next2(int y, int x){
    if(mp[y][x] == "empty" || mp[y][x] == "apple" || (y == tail.first && x == tail.second)) return true;
    else return false;
}

void calculate_next_move(){
    int shortest = 1e9;
    int x = head.second;
    int y = head.first;
    int apple_index = cycle_path[apple.first][apple.second];
    int head_index = cycle_path[head.first][head.second];
    int tail_index = cycle_path[tail.first][tail.second];
    tail_index = reindex(tail_index, head_index);
    apple_index = reindex(apple_index, head_index);
    if(prevdir != LEFT && cycle_path[y][x+1] != -1 && mp[y][x+1] != "snake"){
        int tdex = reindex(cycle_path[y][x+1], head_index);
        if(tdex <= tail_index){
            if(apple_index - tdex - ((apple_index >= tdex)?0:2500) < shortest){
                dir = RIGHT;
                shortest = apple_index - tdex + ((apple_index > tdex)?0:1e7);
            } 
        }
    }
    if(prevdir != UP && cycle_path[y+1][x] != -1 && mp[y+1][x] != "snake"){
        int tdex = reindex(cycle_path[y+1][x], head_index);
        if(tdex <= tail_index){
            if(apple_index - tdex - ((apple_index <= tdex)?0:2500)< shortest){
                dir = DOWN;
                shortest = apple_index - tdex + ((apple_index > tdex)?0:1e7);
            }
        }
    }
    if(prevdir != DOWN && cycle_path[y-1][x] != -1 && mp[y-1][x] != "snake"){
        int tdex = reindex(cycle_path[y-1][x], head_index);
        if(tdex <= tail_index){
            if(apple_index - tdex - ((apple_index <= tdex)?0:2500)< shortest){
                dir = UP;
                shortest = apple_index - tdex + ((apple_index > tdex)?0:1e7);
            }
        }
    }
    if(prevdir != RIGHT && cycle_path[y][x-1] != -1 && mp[y][x-1] != "snake"){
        int tdex = reindex(cycle_path[y][x-1], head_index);
        if(tdex <= tail_index){
            if(apple_index - tdex - ((apple_index <= tdex)?0:2500)< shortest){
                dir = LEFT;
                shortest = apple_index - tdex + ((apple_index > tdex)?0:1e7);
            }   
        }
    }
    if(shortest > 2500 || 2500 - snake.size() < 1500){
        if(auto_next(y+1, x) && prevdir != UP) dir = DOWN;
        else if(auto_next(y-1, x)  && prevdir != DOWN) dir = UP;
        else if(auto_next(y, x+1)  && prevdir != LEFT) dir = RIGHT;
        else if(auto_next(y, x-1)  && prevdir != RIGHT) dir = LEFT;
        else{
            if(auto_next2(y+1, x) && prevdir != UP) dir = DOWN;
            else if(auto_next2(y-1, x)  && prevdir != DOWN) dir = UP;
            else if(auto_next2(y, x+1)  && prevdir != LEFT) dir = RIGHT;
            else if(auto_next2(y, x-1)  && prevdir != RIGHT) dir = LEFT;
        }
    }
    return;
}

void move(int y, int x){
    if(mp[y][x] == "wall" || (mp[y][x] == "snake" && y != tail.first && x != tail.second) || y < 0 || y > 50 || x < 0 || x > 50){
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
        }
        else{
            mp[snake.front().first][snake.front().second] = "empty";
            ClearScreen(tail.second*2, tail.first);
            cout.width(2);
            setconsolecolor(1,0);
            cout << " ";
            snake.pop();
            tail = MP(snake.front().first, snake.front().second);
            head = MP(y,x);
            ClearScreen(tail.second*2, tail.first);
            cout.width(2);
            setconsolecolor(4,8);
            cout << " ";
            mp[y][x] = "snake";
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
        default: 
            break;
    }
    prevdir = dir;
    return;
}

void test_delay(){
    if(kbhit()){
        char key = getch();
        switch(key){
            case 's':
                delay = 's';
                break;
            case 'm':
                delay = 'm';
                break;
            case 'f':
                delay = 'f';
                break;
            default:
                break;
        }
    }
    if(delay == 's') Sleep(20);
    if(delay == 'm') Sleep(1);
    if(delay == 'f') return;
    return;
}
int main(){
    string start = "NO";
    do{
        cin >> start;
    }while(start != "start");
    gameover = false;
    score = 0;
    create_hamiltonian_cycle();
    setup();
    print();
    while(!gameover){
        calculate_next_move();
        logic();
        test_delay();
    }
    ClearScreen(50,54);
    setconsolecolor(15,0);
    setcursor(1,10);
    return 0;
}