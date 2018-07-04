#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "./keyboard/keyboard.h"

//#define FC 5
#define BC 0
#define W 10
#define H 20

#if 0
    []
  [][][]

    []
    []
    [][]

[][]
  [][]

  [][]
  [][]。。。。。。。。


#endif

struct data
{
    int x;
    int y;
};
int FC = 1;
struct data t = {5,0};
int background[H][W] = {};
int cur_shape = 0;
struct shape
{
  int s[5][5];
};

int background[H][W];//背景

//使用5*5的数组来表示图案，有图案用1表示，没有的用0表示
struct shape shape_arr[7]={
{
    0,0,0,0,0,
    0,0,1,0,0,
    0,1,1,1,0,
    0,0,0,0,0,
    0,0,0,0,0,
},
{
    0,0,0,0,0,
    0,0,1,0,0,
    0,0,1,0,0,
    0,0,1,1,0,
    0,0,0,0,0,
},
{
    0,0,0,0,0,
    0,0,1,0,0,
    0,0,1,0,0,
    0,1,1,0,0,
    0,0,0,0,0,
},
{
    0,0,0,0,0,
    0,0,1,0,0,
    0,0,1,0,0,
    0,0,1,0,0,
    0,0,0,0,0,
},
{
    0,0,0,0,0,
    0,1,1,0,0,
    0,0,1,1,0,
    0,0,0,0,0,
    0,0,0,0,0,
},
{
    0,0,0,0,0,
    0,0,1,1,0,
    0,1,1,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
},
{
    0,0,0,0,0,
    0,1,1,0,0,
    0,1,1,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
}
};


void drop_element(int x , int y , int c)
{
    x = x * 2;
    x++;
    y++;
    printf("\033[?25l");//隐藏光标
    printf("\033[%d;%dH", y, x);//控制光标位置
    printf("\033[3%dm\033[4%dm", c, c);//设置颜色
    printf("[]");//输出方块
    fflush(stdout);//刷新缓存
    printf("\033[0m");//关闭设置的属性
}

void drop_shape(int x,int y,struct shape p,int c)
{
    int i = 0;
    for(i = 0;i < 5;i++)
    {
        int j = 0;
        for(j = 0;j < 5;j++)
        {
            if(p.s[i][j] != 0)
            {
                drop_element(x + j,y + i, c);
            }
        }
    }
}

void drop_back(void)
{
    int i = 0;
    for(i = 0;i < H;i++)
    {
        int j = 0;
        for(j = 0;j < W;j++)
        {
            if(background[i][j] == 0)
            {
                drop_element(j,i,BC);
            }
            else
            {
                drop_element(j,i,background[i][j]);
            }
        }
    }
}

void set_back(struct data * t,struct shape p)
{
    int i = 0;
    for(i = 0;i < 5;i++)
    {
        int j = 0;
        for(j = 0;j < 5;j++)
        {
            if(p.s[i][j] != 0)
            {
                background[t->y+i][t->x+j] = FC;
            }
        }
    }
}

int can_move(int x, int y,struct shape p)
{
    int i = 0;
    for(i = 0;i < 5 ; i++)
    {
        int j = 0;
        for(j = 0;j < 5;j++)
        {
            if(p.s[i][j] == 0)
            {
                continue;
            }
            if(x + j >= W)
            {
                return 0;
            }
            if(y + i >= H)
            {
                return 0;
            }
            if(x + j < 0)
            {
                return 0;
            }
            if(background[y+i][x+j] != 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

void mclean_line(void)
{

    int i = 0;
    for(i = 0;i < H;i++)
    {
        int total = 0;
        int j = 0;
        for(j = 0;j < W;j++)
        {
            if(background[i][j] != 0)
            {
                total++;
            }
        }
        if(total == W)
        {
            int k = 0;
            for(k = i;k > 0;k--)
            {
                memcpy(background[k],background[k - 1],sizeof(background[k]));
            }
            memset(background[0],0x00,sizeof(background[0]));
        }
    }
}

void tetris_timer(struct data *t)
{
    drop_shape(t->x,t->y,shape_arr[cur_shape],BC);
    if(can_move(t->x,t->y+1,shape_arr[cur_shape]))
    {
        t->y++;
    }
    else
    {
        set_back(t,shape_arr[cur_shape]);
        mclean_line();
        drop_back();
        do
        {
            FC = rand()%7;
        }
        while(FC == BC);
        t->y = 0;
        t->x = 0;
        cur_shape = rand()%7;
        

    }
    //t->y++;
    drop_shape(t->x,t->y,shape_arr[cur_shape],FC);

}

struct shape turn_90(struct shape p)
{
    struct shape t;
    int i = 0;
    for(i = 0;i < 5;i++)
    {
        int j = 0;
        for(j = 0; j < 5;j++)
        {
            t.s[i][j] = p.s[4-j][i];
        }
    }
    return t;
}

int tetirs(struct data * t)
{
    int ret = 0;
    int c = get_key();
    if(is_up(c))
    {
        drop_shape(t->x,t->y,shape_arr[cur_shape],BC);
        shape_arr[cur_shape] = turn_90(shape_arr[cur_shape]);
        if(can_move(t->x,t->y,shape_arr[cur_shape]) == 0)
        {
            shape_arr[cur_shape] = turn_90(shape_arr[cur_shape]);
            shape_arr[cur_shape] = turn_90(shape_arr[cur_shape]);
            shape_arr[cur_shape] = turn_90(shape_arr[cur_shape]);
        }
        drop_shape(t->x,t->y,shape_arr[cur_shape],FC);
    }
    else if(is_left(c))
    {
        drop_shape(t->x,t->y,shape_arr[cur_shape],BC);
        if(can_move(t->x-1,t->y,shape_arr[cur_shape]))
            t->x--;
        drop_shape(t->x,t->y,shape_arr[cur_shape],FC);
        
    }
    else if(is_right(c))
    {
        drop_shape(t->x,t->y,shape_arr[cur_shape],BC);
        if(can_move(t->x+1,t->y,shape_arr[cur_shape]))
            t->x++;
        drop_shape(t->x,t->y,shape_arr[cur_shape],FC);

    }
    else if(is_down(c))
    {
        drop_shape(t->x,t->y,shape_arr[cur_shape],BC);
        if(can_move(t->x,t->y+1,shape_arr[cur_shape]))
            t->y++;
        drop_shape(t->x,t->y,shape_arr[cur_shape],FC);

    }
    else if(is_esc(c))
    {
        ret = 1;
    }
    return ret;
}

void handler(int s)
{
        tetris_timer(&t);

}

int main()
{
    init_keyboard();
    drop_back();

    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGALRM,&act,NULL);
    struct itimerval it;
    it.it_value.tv_sec = 0;
    it.it_value.tv_usec = 1;
    it.it_interval.tv_sec = 1;
    it.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL,&it,NULL);
    while(1)
    {
        //tetris_timer(&t);
        if(tetirs(&t) == 1)
        {
            break;
        }
        //sleep(1);
    }
    printf("\033[?25H");
    recover_keyboard();
}


