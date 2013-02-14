#include "user.h"
#include "net.h"
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <locale.h>
#include "include.h"

/******************************
 *
 * 函数功能：窗口初始化
 *
 * ***************************/
int WindowInit()
{
    setlocale(LC_ALL,"");
    initscr();
    ColorInit();
}

int MessageBox(int x,int y,char *message,int length,int sign)
{
    int result = 0;
    while(result < 5)
    {
        move(y/2 + 2 - result,x/2 - 19);
        printw("%-38s"," ");
        result++;
    }
    result = 0;
    attron(COLOR_PAIR(6));
    move(y/2 - 3,x/2 - 19);
    hline(ACS_HLINE,38); 
    move(y/2 - 2,x/2 - 5);
    printw("提      醒");
    move(y/2,x/2 - length);
    printw("%-*s",length,message);
    if (MSG_OK == sign)
    {
        move(y/2 + 2,x/2 - 10);
        attron(COLOR_PAIR(3));
        printw("  %-4s","OK");
        attroff(COLOR_PAIR(3));
        move(y/2 + 2,x/2 + 6);
        attron(COLOR_PAIR(4));
        printw("%6s","Cancel");
        attroff(COLOR_PAIR(4));
    }
    else
    {
        move(y/2 + 2,x/2 - 10);
        attron(COLOR_PAIR(4));
        printw("  %-4s","OK");
        attroff(COLOR_PAIR(4));
        move(y/2 + 2,x/2 + 6);
        attron(COLOR_PAIR(3));
        printw("%6s","Cancel");
        attroff(COLOR_PAIR(3));
    }
    move(y/2 - 2,x/2 - 20);
    attron(COLOR_PAIR(6));
    vline(ACS_VLINE,5);
    move(y/2 + 3,x/2 - 19);
    hline(ACS_HLINE,38);
    move(y/2 - 2,x/2 + 19);
    vline(ACS_VLINE,5);
    refresh();
    attroff(COLOR_PAIR(6));
    return 0;
}

/****************************
 *
 * 函数功能：登陆界面
 *
 *
 * **************************/
int Cli_Login(int x,int y,char name[USERNAME_SIZE],char passwd[USERPASSWD_SIZE])
{
    clear();
    leaveok(stdscr,0);
    attron(COLOR_PAIR(1));
    box(stdscr,0,0);
    move(y/2-2,x/2-10);
    printw("账号:");
    move(y/2,x/2-10);
    printw("密码:");
    move(y/2-2,x/2-4);
    scanw("%s",name);
    move(y/2,x/2-4);
    scanw("%s",passwd);
    return 0;
}

/****************************
 *
 * 函数功能：注册新账号
 *
 *
 * **************************/
int Cli_Apply(int x,int y,char name[USERNAME_SIZE],char passwd[USERPASSWD_SIZE])
{
    char passwd1[USERPASSWD_SIZE];
    char passwd2[USERPASSWD_SIZE];
loop:    clear();
    leaveok(stdscr,0);
    attron(COLOR_PAIR(1));
    box(stdscr,0,0);
    move(y/2-2,x/2-10);
    printw("新用户名:");
    move(y/2,x/2-10);
    printw("密码:");
    move(y/2+2,x/2-10);
    printw("确认密码:");
    move(y/2-2,x/2);
    scanw("%s",name);
    move(y/2,x/2);
    scanw("%s",passwd1);
    move(y/2+2,x/2);
    scanw("%s",passwd2);
    if (0 == strcmp(passwd1,passwd2))
        strcpy(passwd,passwd1);
    else
    {
        move(y/2+4,x/2-18);
        printw("两次密码不相同,请重新输入,按任意键继续");
        getch();
        goto loop;
    }
    return 0;
}

/********************************
 *
 * 函数功能：客户端欢迎界面
 * 返回值：1 - 登陆
 *         2 - 注册新账号
 *
 * ****************************/
int Cli_Welcome()
{
    int x,y;
    int key;
    int sign = 1;
    keypad(stdscr,1);
    leaveok(stdscr,1);
    while(1)
    {
        clear();
        refresh();
        attron(COLOR_PAIR(1));
        GetSize(&x,&y);
        box(stdscr,0,0);
        move(y/2 -10,x/2 - 5);
        printw("Mychat v1.0");
        attroff(COLOR_PAIR(2));
        if (sign == 1)
            attron(COLOR_PAIR(3));
        else
            attron(COLOR_PAIR(7));
        move(y/2-2,x/2-4);
        printw("登    陆");
        if (sign == 1)
            attroff(COLOR_PAIR(3));
        else
            attroff(COLOR_PAIR(7));
        if (sign == 2)
            attron(COLOR_PAIR(3));
        else
            attron(COLOR_PAIR(7));
        move(y/2,x/2-4);
        printw("新建账号");
        if (sign == 2)
            attroff(COLOR_PAIR(3));
        else
            attroff(COLOR_PAIR(7));
        if (sign == 3)
            attron(COLOR_PAIR(3));
        else
            attron(COLOR_PAIR(7));

        move(y/2+2,x/2-4);
        printw("退    出");
        if (sign == 3)
            attroff(COLOR_PAIR(3));
        else
            attroff(COLOR_PAIR(7));
        move(y - 2,x/2 - 17);
        printw("By long, At 2013.2.6, QQ:947660101");
        refresh();
        key = 0;
        key = getch();
        switch(key)
        {
            case KEY_DOWN:
                if (sign < 3)
                    sign++;
                break;
            case KEY_UP:
                if (sign > 1)
                    sign--;
                break;
            case 108:
            case 10:
            case 109:
            {
                if (1 == sign)
                    return 1;
                else if (2 == sign)
                    return 2;
                else
                    return 3;
            }
        }
    }
    leaveok(stdscr,0);
    return 0;
}

/*********************************
 *
 * 函数功能：获取窗口大小
 * 参数：x - 窗口行数
 *       y - 窗口列数
 * 返回值：成功 - 0
 *         错误 - 1
 *
 * ******************************/
int GetSize(int *x,int *y)
{
    struct winsize size;
    if (0 == isatty(STDOUT_FILENO))
        return 1;
    if (ioctl(STDOUT_FILENO,TIOCGWINSZ,&size) < 0)
    {
        perror("ioctl TIOCGWINSZ error");
        return 1;
    }
    *y = size.ws_row;
    *x = size.ws_col;
    return 0;
}

/********************************
 *
 * 函数功能：颜色初始化
 *
 * *****************************/
int ColorInit()
{
    if (OK != start_color())
        return -1;
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    init_pair(2,COLOR_BLACK,COLOR_GREEN);
    init_pair(3,COLOR_RED,COLOR_GREEN);
    init_pair(4,COLOR_RED,COLOR_WHITE);
    init_pair(5,COLOR_GREEN,COLOR_BLACK);
    init_pair(6,COLOR_RED,COLOR_BLACK);
    init_pair(7,COLOR_RED,COLOR_WHITE);
}
/********************************
 *
 * 函数功能：客户端界面窗口
 * 参数：当前窗口行数、列数
 * 返回值：成功 - 0
 *         失败 - -1
 *
 * *****************************/
int Cli_Windows(int *x,int *y)
{
    int x1 = 0,y1 = 0;
    char time[21];
    getyx(stdscr,y1,x1);
    attron(COLOR_PAIR(1));
    *x = *y = 0;
    leaveok(stdscr,1);
    GetSize(x,y);
    GetTime(time);
    box(stdscr,0,0);        //外边框
    move(1,*x/2-10);
    printw("多人聊天软件客户端 v1.0");
    move(2,*x-25);
    printw("时间:%-19s",time);
    move(3,1);
    hline(ACS_HLINE,*x-2);
    move(4,1);
    printw("   好友列表");
    move(5,1);
    hline('-',15);
    move(4,16);
    vline(ACS_VLINE,*y-5);
    move(4,*x - 16);
    vline(ACS_VLINE,*y-5);
    move(4,*x-12);
    printw("好友信息");
    move(5,*x-15);
    hline('-',14);
    move(*y-3,17);
    hline(ACS_HLINE,*x - 33);
    attroff(COLOR_PAIR(1));
    refresh();
    move(y1,x1);
    leaveok(stdscr,0);
    return 0;
}

/*******************************
 *
 * 函数功能：服务器界面窗口a
 *
 * ****************************/
int Ser_windows(int *x,int *y,int sumofonlineuser,int sumofuser)
{
    int x1,y1;
    char time[21];
    getyx(stdscr,y1,x1);   //获取光标当前位置
    attron(COLOR_PAIR(1));
    *x =*y = 0;
    GetTime(time);
    GetSize(x,y);
    leaveok(stdscr,1);
    box(stdscr,0,0);
    move(1,*x/2-14);
    printw("多人聊天软件服务器端 v1.0");
    move(2,*x-25);
    printw("时间:%-19s",time);
    move(3,1);
    hline(ACS_HLINE,*x-2);
    move(4,1);
    printw("   用户列表");
//    move(6,1);
//    printw("你好我是谁你是");
    move(5,1);
    hline('-',15);
    move(4,16);
    vline(ACS_VLINE,*y-5);
    move(4,*x - 20);
    vline(ACS_VLINE,*y-5);
    move(4,*x-15);
    printw("服务器信息");
    move(6,*x-19);
    printw("  用户总数：%-4d",sumofuser);
    move(7,*x-19);
    printw("在线用户数：%-4d",sumofonlineuser);
    move(11,*x-19);
    hline(ACS_HLINE,18);
    move(12,*x - 14);
    printw("实时动态");
    move(13,*x - 19);
    hline('-',18);
    move(5,*x-19);
    hline('-',18);
    move(4,32);
    vline(ACS_VLINE,*y-5);
    move(8,33);
    hline(ACS_HLINE,*x - 33 - 20);
//    move(6,17);
//    printw("你好我是谁你是");
    move(4,20);
    printw("好友列表");

    move(4,(*x-33-19)/2 - 10 + 33);
    printw("用户信息");
    move(5,33);
    hline('-',*x - 53);
    move(5,17);
    hline('-',15);
    move(*y-3,33);
    hline(ACS_HLINE,*x - 53);
    leaveok(stdscr,0);
    attroff(COLOR_PAIR(1));
    refresh();
    move(y1,x1);   //恢复光标
    return 0;
}

/******************************
 *
 * 函数功能：在对应区域显示好友列表
 * 参数：x,y - 窗口尺寸
 *       friendslist - 好友列表
 *       num - 被选中好友编号（从1开始）
 *       sum - 好友户数
 *       name - 返回选中好友用户名
 * 返回值：成功- 0
 *         失败 - -1
 *
 * ***************************/
int Cli_DisplayFriendList(int x,int y,struct Cli_Friendslist *friendslist,int num,int sum,char name[USERNAME_SIZE])
{
    int cur = 6;
    int temp = 0;
    int x1 = 0,y1 = 0;
    getyx(stdscr,y1,x1);
    leaveok(stdscr,1);
    while(cur < y - 1)
    {
        move(cur,1);
        printw("%-*s",15," ");
        cur++;
    }
    move(y1,x1);
//    refresh();
    if (NULL == friendslist->next)
        return 0;
    if (num < y - 7)
    {
        cur = 6;
        while(cur < y - 1)
        {
            if (NULL == friendslist)
                break;
            if (temp == num)      //如果是被选中的用户，则反色显示
            {
                attron(COLOR_PAIR(2));
                move(cur,1);
                printw("%-*s",15,friendslist->name);
                attroff(COLOR_PAIR(2));
                strcpy(name,friendslist->name);
            }
            else     //如果好友不是被选中的用户
            {
                if (friendslist->online == 1)   //在线用户
                {
                    attron(COLOR_PAIR(6));
                    move(cur,1);
                    printw("%-*s",15,friendslist->name);
                    attroff(COLOR_PAIR(6));
                }
                else      //离线用户
                {
                    attron(COLOR_PAIR(5));
                    move(cur,1);
                    printw("%-*s",15,friendslist->name);
                    attroff(COLOR_PAIR(5));
                }
            }
            refresh();
            friendslist = friendslist->next;
            cur++;
            temp++;
        }
    }
    else
    {
        cur = y - 2;
        while(temp < num && NULL != friendslist->next)
        {
            friendslist = friendslist->next;
            temp++;
        }
        while(cur >= 6)
        {
            if (NULL == friendslist)
                break;
            if (temp == num)
            {
                attron(COLOR_PAIR(2));
                move(cur,1);
                printw("%-*s",15,friendslist->name);
                attroff(COLOR_PAIR(2));
                strcpy(name,friendslist->name);
            }
            else     //如果好友不是被选中的用户
            {
                if (friendslist->online == 1)   //在线用户
                {
                    attron(COLOR_PAIR(6));
                    move(cur,1);
                    printw("%-*s",15,friendslist->name);
                    attroff(COLOR_PAIR(6));
                }
                else      //离线用户
                {
                    attron(COLOR_PAIR(5));
                    move(cur,1);
                    printw("%-*s",15,friendslist->name);
                    attroff(COLOR_PAIR(5));
                }
            }
            refresh();
            friendslist = friendslist->front;
            cur--;
            temp--;
               
        }
    }
    refresh();
    move(y1,x1);
}

/*************************************
 *
 * 函数功能：显示好友信息
 * 参数：x,y - 窗口大小
 *       user - 用户链表
 *       name - 好友用户名
 * 返回值：成功- 0
 *         失败 - -1
 *
 * **********************************/
int Cli_DisplayUserDate(int x,int y,struct User_List *user,char name[USERNAME_SIZE])
{
    int cur = 5;
    int x1,y1;
    if (NULL != user->next)
    {
        user = user->next;
        getyx(stdscr,y1,x1);    //获取光标当前坐标
        while(NULL != user)
        {
            if (0 == strcmp(name,user->user.name))
                mvaddstr(cur,x - 15,user->user.name);
            user = user->next;
            cur++;
        }
        refresh();
        move(y1,x1);
    }
    return 0;
}

/************************************
 *
 * 函数功能：服务器显示用户列表
 * 参数：x,y - 窗口大小
 *       list - 用户列表
 *       num - 被选中用户编号
 *       sum - 总用户数
 *       sign - 用户在线标识
 *       name - 返回当前选择用户名
 * ***********************************/
int Ser_DisplayUserList(int x,int y,char list[][USERNAME_SIZE],int num,int sum,int sign[200],char name[USERNAME_SIZE])
{
    int cur = 6;
    int temp = 0;
    int len = 0;
    int x1,y1;
    char name_temp[USERNAME_SIZE];
    num -= 1;
    strcpy(list[0],"整体动态");
    if (num > sum)
        return 0;
    if (num > (y - 8))    //如果选中的好友序号大于一屏所能显示的好友数
        temp = num - (y -8);    //则将该好友显示在最低
    while(cur <= (y - 2))     
    {
        if (strcmp(list[temp],"") != 0)
        {
            getyx(stdscr,y1,x1);   //获取光标当前坐标
            if (temp == num)      //如果是被选中的用户，则反色显示
            {
                attron(COLOR_PAIR(2));
                move(cur,1);
                printw("%-*s",15,list[temp]);
                attroff(COLOR_PAIR(2));
                strcpy(name,list[temp]);
            }
            else     //如果好友不是被选中的用户
            {
                if (sign[temp] == 1)   //在线用户
                {
                    attron(COLOR_PAIR(6));
                    move(cur,1);
                    printw("%-*s",15,list[temp]);
                    attroff(COLOR_PAIR(6));
                }
                else      //离线用户
                {
                    attron(COLOR_PAIR(5));
                    move(cur,1);
                    printw("%-*s",15,list[temp]);
                    attroff(COLOR_PAIR(5));
                }
            }
            refresh();
            move(y1,x1);    //恢复光标
            temp++;
            cur++;
        }
        else
            break;
    }
}

/************************************
 *
 * 函数功能：服务器端显示用户好友列表
 * 参数：x,y - 窗口大小
 *       friends - 好友列表
 *       num - 当前被选中好友编号
 *       sum - 好友总数
 *       name - 返回好友用户名
 *
 * ********************************/
int Ser_DisplayFriendList(int x,int y,char friends[][USERNAME_SIZE],int num,int sum,char name[USERNAME_SIZE])
{
    int cur = num;
    int line = 6;
    int temp = 0;
    while(line < y - 1)
    {
        move(line,17);
        printw("%15s"," ");
        line++;
    }
    refresh();
    if (num >= y - 7)
    {
        line = y - 2;
        while(line > 5)
        {
            if (cur == num)    //若为选中对象，反显
            {
                attron(COLOR_PAIR(2));
                move(line,17);
                printw("[%-*s]",13,friends[cur]);
                attroff(COLOR_PAIR(2));
                strcpy(name,friends[cur]);
            }
            else
            {
                attron(COLOR_PAIR(5));
                move(line,17);
                printw("[%-*s]",13,friends[cur]);
                attroff(COLOR_PAIR(5));
            }
            line--;
            cur--;
        }
        refresh();
    }
    else
    {
        line = 6;
        while(line <= sum + 6 && line < y - 1)
        {
            if (temp == num)   //若为想选中对象，反显
            {
                attron(COLOR_PAIR(2));
                move(line,17);
                printw("[%-*s]",13,friends[temp]);
                attroff(COLOR_PAIR(2));
                strcpy(name,friends[temp]);
            }
            else
            {
                attron(COLOR_PAIR(5));
                move(line,17);
                printw("[%-*s]",13,friends[temp]);
                attroff(COLOR_PAIR(5));
            }
            line++;
            temp++;
        }
    }
}

/*************************************
 *
 * 函数功能：键盘控制
 * 参数：num - 选择对象编号
 *       sign - 功能选择 从0开始
 *       logout - 退出登录标识,为真有效
 * 
 * *********************************/
int KeyboardControl(int *num,int *max_num,int *sign,int *logout,char *message,int *message_sign,int *addfriend_sign,char *addfriendsname,int *messageboxsign,int *messageboxnum,int *change)
{
    int key = 0;
    int x,y;
    int sign_temp = 0;
    char temp[DATELEN];
    while(1)
    {
        GetSize(&x,&y);
        if (3 != *sign)   //若功能不为聊天输入模式，则检测按键
        {
            key = 0; 
            keypad(stdscr,1);
            leaveok(stdscr,1);
            key = getch();
            keypad(stdscr,0);
            leaveok(stdscr,0);
            *change = 1;
            switch(key)
            {
                case 10:
//                    if (0 == *sign)
                        *sign = 3;
                    break;
                case 'h':             //向左移动
                case KEY_LEFT:
                    if (1 == *messageboxsign)
                        *messageboxnum = 0;
                    else
                    {
                        if (*sign > 0)
                            (*sign)--;
                        if (1 != *sign)
                            num[1] = 0;
                    }
                    break;
                case 'l':             //向右移动
                case KEY_RIGHT:
                    if (1 == *messageboxsign)
                        *messageboxnum = 1;
                    else
                    {
                        if (*sign < 3)
                            (*sign)++;
                        if (1 != *sign)
                            num[1] = 0;
                    }
                    break;
                case 'k':              //向上移动
                case KEY_UP:
                    if (0 < num[*sign])
                        num[*sign]--;
                    if (1 != *sign)
                        num[1] = 0;
                    break;
                case 'j':              //向下移动
                case KEY_DOWN:
                    if (num[*sign] < max_num[*sign])
                        num[*sign]++;
                    else if (num[*sign] > max_num[*sign])
                        num[*sign] = max_num[*sign];
                    if (1 != *sign)
                        num[1] = 0;
                    break;
                case '1':             //数字键  - 1
                    *sign = 3;
                    break;
                case '2':             //数字键  - 2
                    *sign = 0;
                    break;
                case '3':             //数字键  - 3
                    *sign = 1;
                    break;
                case '4':             //数字键  - 4
                    *messageboxsign = 1;
//                    *sign = 2;
                    break;
                case '5':             //数字键  - 5
                    *logout = 1;
            //     return 0;
            }
//            usleep(1000);
        }
        else     //若功能选择为聊天呼入模式，则获取用户输入
        {
            memset(temp,0x0,sizeof(temp));
            while(0 != *message_sign);   //等待发送缓冲区为空
            move(y-2,17);
            printw("            ");
            move(y - 2,17);
            echo();
            keypad(stdscr,0);
            leaveok(stdscr,0);
            getstr(temp);
            keypad(stdscr,1);
            leaveok(stdscr,1);
            noecho();
            //输入'q'(quit)或'fl'(friend list)
            //则退出聊天模式，进入好友列表选择模式.
            if (0 == strcmp(temp,CM_QUIT))               //退出
                *logout = CLOSE;
            else if (0 == strcmp(temp,CM_MENU_FRIENDLIST))   //返回好友列表
                *sign = 0;
            else if (0 == strncmp(temp,CM_ADDFRIEND,4))   //添加好友
            {
                int num = 4;
                while(0 != *addfriend_sign);
                memset(addfriendsname,0x0,USERNAME_SIZE*sizeof(char));
                while(temp[num] != '\0')
                {
                    addfriendsname[num - 4] = temp[num];
                    num++;
                }
                addfriendsname[num] = '\0';
                *addfriend_sign = 1;
            }
            else if (0 == strncmp(temp,CM_DELFRIEND,3))   //删除好友KE
            {
                int num = 4;
                while(0 != *addfriend_sign);
                memset(addfriendsname,0x0,USERNAME_SIZE*sizeof(char));
                while(temp[num] != '\0')
                {
                    addfriendsname[num - 4] = temp[num];
                    num++;
                }
                addfriendsname[num] = '\0';
                *addfriend_sign = 2;
            }
            else
            {
                strcpy(message,temp);
                *message_sign = 1;
            }
        }
    }
    return 0;
}

/******************************
 *
 * 函数功能：显示通信信息
 * 参数：
 *      x,y - 屏幕尺寸
 *      user - 用户列表
 *      username - 欲显示通信信息的用户名
 *      friendsname - 欲显示通信信息的好友用户名
 * 返回值：
 *      -1 - 没有该用户
 *      -2 - 没有该好友
 *      -3 - 没有聊天信息
 *       0 - 正常运行
 *
 * **************************/
int Ser_DisPlayMsg(int x,int y,struct User_List *user,char username[USERNAME_SIZE],char friendsname[USERNAME_SIZE])
{
    int msgdis_max = y - 7;
    int num = 0;
    struct User_List *temp = user;
    struct Friend *friends = NULL;
    struct MessageLog *msglog = NULL;
    int x1,y1;
    while(NULL != temp)    //寻找用户名
    {
        if (0 == strcmp(temp->user.name,username))
            break;
        if (NULL == temp->next)
            return -1;
        temp = temp->next;
    }
    friends = &(temp->user.friends);
    while(NULL != friends)   //寻找对应好友
    {
        if (0 == strcmp(friends->name,friendsname))
            break;
        if (NULL == friends->next)
            return -2;
        friends = friends->next;
    }
    msglog = friends->messagelog.next;
    int z = 9;
    while(z < y - 3)
    {
        move(z,33);
        printw("%-*s",x - 54," ");
        z++;
    }
    refresh();
    getyx(stdscr,y1,x1);   //获取光标当前位置
    while(num < msgdis_max)
    {
        if (NULL != msglog)
        {
            move(num + 9,33);
            printw("%+*s",x - 54," ");
            move(num + 9,33);
            if (2 == msglog->owner) //若是自身发言，显示在右边
                printw("%+*s",x - 54,msglog->message);
            else
                printw("%-*s",x - 54,msglog->message);
//            if (NULL == msglog->next)
//                break;
            msglog = msglog->next;
            num++;
        }
        else
        {
            move(y1,x1);
            refresh();
            return 0;
        }
    }
    move(y1,x1);
    refresh();
}

/****************************************
 *
 * 函数功能：客户端显示聊天记录
 *
 * ************************************/
int Cli_DisPlayMsg(int x,int y,struct Cli_Friendslist *friendslist,char friendsname[USERNAME_SIZE])
{
    int x1 = 0,y1 = 0;
    int msgdis_max = 20;
    int num = 0;
    int rows_cur = 0;
    int sumofmsg = 0;
    struct MessageLog *messagelog = NULL;
    if (NULL == friendslist->next)
        return 1;
    while(NULL != friendslist)      //寻找对应好友
    {
        if (0 == strcmp(friendslist->name,friendsname))
            break;
        friendslist = friendslist->next;
    }
    if (NULL == friendslist)
        return 1;
    else
    {
        getyx(stdscr,y1,x1);
        leaveok(stdscr,1);
        num = 4;
        while(num < y - 4)
        {
            move(num,17);
            printw("%-*s",x - 33," ");
            num++;
        }
        messagelog = &friendslist->messagelog;
        while(NULL != messagelog->next)
        {
            messagelog = messagelog->next;
            sumofmsg++;
        }
        if (sumofmsg > y - 7)
        {
            rows_cur = y - 4;
            while(rows_cur >= 3)
            {
                move(rows_cur,17);
                if (2 == messagelog->owner) //若是自身发言，显示在右边
                    printw("%-*s",x - 33,messagelog->message);
                else
                    printw("%+*s",x - 33,messagelog->message);
                messagelog = messagelog->front;
                rows_cur--;
            }
        }
        else
        {
            messagelog = friendslist->messagelog.next;
            rows_cur = 4;
            while(rows_cur < y - 4)
            {
                if (NULL == messagelog)
                    break;
                move(rows_cur,17);
                if (2 == messagelog->owner) //若是自身发言，显示在右边
                    printw("%-*s",x - 33,messagelog->message);
                else
                    printw("%+*s",x - 33,messagelog->message);
                messagelog = messagelog->next;
                rows_cur++;
            }
        }
        refresh();
        move(y1,x1);
        leaveok(stdscr,0);
    }
    return 0;
}








