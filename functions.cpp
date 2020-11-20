#include <iostream>
#include <curses.h>
#include "common.hpp"
using namespace std;

// 충돌에 대한 판정 (미완성)
// 아래 충돌은 문제 없음 위 충돌 및 윈도우 밖으로 안나가도록 출동 판정 업그레이드 필요
// 볃에 대한 충돌만 인정되므로 적 오브젝트에 대한 충돌 판정 필요
int is_move_ok(int y, int x){   
    int compare_ch;
    compare_ch = mvinch(y,x); // 주어진 위치 문자 return
    return !((compare_ch=='*' || y >LINES-2 || y<0 || x > COLS- 70 - 8 || x < 0));  // 문자 제한, 윈도우 제한
}

// 플레이어 움직임
// ch 키보드 입력에 따라 이동
struct Player move2direction(int ch, struct Player player){
    switch (ch)    {
        case KEY_UP:
            player.jump_flag = 1; player.jump_counter=player.jump_hight; break; //jump flag on
        case KEY_DOWN:
            player.down_flag = 1; player.down_counter=player.down_time; break;
        case KEY_LEFT:
            if (is_move_ok(player.y,player.x - 1)){player.x = player.x - 1;}; break;
        case KEY_RIGHT:
            if (is_move_ok(player.y,player.x + player.shape_size_y + 1)){player.x = player.x + 1;}; break;
    }
    return player;
}

// 중력 (자동적으로 아래로 이동)
// 지상이 아래이기 때문에 점프같은 액션으로 위로 갈 경우 아래로 자연적으로 움직임 필요
struct Player gravityOfPlayer(int counter, struct Player player){
    int maxtime = 5;
    if(counter%maxtime==maxtime-1){
        if (is_move_ok(player.y + 1,player.x)){player.y = player.y + 1;}
    }
    return player;
}

// 점프 (jump flag가 1일 때 위로 이동)
struct Player jumppingOfPlayer(int counter, struct Player player){
    int maxtime = 5;
    if(counter%maxtime==maxtime-1){
        if (is_move_ok(player.y - 1,player.x)){player.y = player.y - 1;}
        player.jump_counter--;
    }
    if(player.jump_counter<=0){
        player.jump_flag=0;
        player.floating_flag=1;
        player.jump_counter=player.jump_time;
    }
    return player;
}

// 장애물 등의 오브젝트가 자연적으로 왼쪽으로 이동
struct obj_RAPA moveObj(int counter, struct obj_RAPA rapa){
    int maxtime = 3;
    if(counter%maxtime==maxtime-1){
        rapa.x = rapa.x - 1;
    }
    return rapa;
}

// 맵 출력
void display_map(char **map, int max_y, int max_x){
    for(int i=0; i<max_y; i++){
        for (int j=0; j<max_x; j++){
            mvaddch(i,j,map[i][j]);
        }
    }
}

// 게임 정보 출력
void display_information(objAll obj){
    string timestring = "time: " + to_string(obj.timeCounter);  //show time
    mvaddstr(1, 1, timestring.c_str()); 
}

void flow_map_bg(char**map, int max_y, int max_x){
    char temp=map[2][0];
    for (int j=0; j<max_x-1; j++){
        map[2][j] = map[2][j+1];
        map[max_y-1][j] = map[max_y-1][j+1];
    }

    // map[0][max_x-1] = temp;
    map[2][max_x-1] = temp;
    map[max_y-1][max_x-1] = temp;

    // if(map[2][max_x-2]=='-' && map[2][max_x-3]=='-'){
    //     map[2][max_x-1] = '*';
    //     map[max_y-1][max_x-1] = '*';
    // }else{
    //     map[2][max_x-1] = '-';
    //     map[max_y-1][max_x-1] = '-';
    // }
}
/*     for(int i=0; i<max_y; i++){
        for (int j=0, k =0; j<max_x; j++){
            map[i][j] = ' ';
            // if(i==0 || i==max_y-1 || i==2){
            //     if(j%3==0){map[i][j] = '*';}
            //     else{map[i][j] = '-';}

            // 1. RTC control system (7.13-8.28)
            k = (j+1)%33;
            switch (k)
            {
            case 1: map[i][j] = '1'; break;
            case 2: map[i][j] = '.'; break;
            case 3: map[i][j] = '+'; break;
            case 4: map[i][j] = 'R'; break;
            case 5: map[i][j] = 'T'; break;
            case 6: map[i][j] = 'C'; break;
            case 7: map[i][j] = 'c'; break;
            case 8: map[i][j] = 'o'; break;
            case 9: map[i][j] = 'n'; break;
            case 10: map[i][j] = 't'; break;
            case 11: map[i][j] = 'r'; break;
            case 12: map[i][j] = 'o'; break;
            case 13: map[i][j] = 'l'; break;
            case 14: map[i][j] = '+'; break;
            case 15: map[i][j] = 's'; break;
            case 16: map[i][j] = 'y'; break;
            case 17: map[i][j] = 's'; break;
            case 18: map[i][j] = 't'; break;
            case 19: map[i][j] = 'e'; break;
            case 20: map[i][j] = 'm'; break;
            case 21: map[i][j] = '+'; break;
            case 22: map[i][j] = '('; break;
            case 23: map[i][j] = '7'; break;
            case 24: map[i][j] = '.'; break;
            case 25: map[i][j] = '1'; break;
            case 26: map[i][j] = '3'; break;
            case 27: map[i][j] = '-'; break;
            case 28: map[i][j] = '8'; break;
            case 29: map[i][j] = '.'; break;
            case 30: map[i][j] = '2'; break;
            case 31: map[i][j] = '8'; break;
            case 32: map[i][j] = ')'; break;
            default:
                break;
            // }
        }
    }
}
} */