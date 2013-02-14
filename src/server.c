#include "windows.h"
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <curses.h>
#include "include.h"
#include <sys/epoll.h>
#include <netdb.h>
#include "net.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

pthread_mutex_t mut;

void *Display(void *argv1);
void *NewUserConnect(void *argv1);
void *Keyboard(void *argv1);

int main()
{
    struct arg_ser_key argv_key1,*argv_key;
    struct arg_ser_dis argv_dis1,*argv_dis;
    struct  User_List *user = &list;
    struct sockaddr_in cli_addr[USER_MAX];
    struct epoll_event event;
    struct epoll_event *events;

    argv_key = &argv_key1;
    argv_dis = &argv_dis1;
    pthread_t pth_t[USER_MAX],pth,pth_dis;
    
    int sfd = 0,s = 0,efd = 0;
    int sign_menu = 0;    //功能标识
    int num[4] = {0};     //各模块被选中成员编号
    int num_max[4] = {0};   //各模块最大成员数
    int logout = 0;   //退出标识
    int sumofuser = 0;
    int fd_ = 0,fd[USER_MAX] = {0};
    int sumofcli = 0;
    int message_sign = 0;
    int messageboxsign = 0;
    int messageboxnum = 0;
    int change = 0;

    char name_cur[USERNAME_SIZE];
    char message[USER_MAX][DATELEN];
    char name[USER_MAX][USERNAME_SIZE];
    char passwd[USER_MAX][USERPASSWD_SIZE];
    char temp[2][USERNAME_SIZE];
    char message_send[DATELEN];
    //各类初始化
    argv_key->num = num;
    argv_key->num_max = num_max;
    argv_key->sign = &sign_menu;
    argv_key->logout = &logout;
    argv_key->message = message_send;
    argv_key->message_sign = &message_sign;
    argv_key->messageboxsign = &messageboxsign;
    argv_key->messageboxnum = &messageboxnum;
    argv_key->change = &change;

    argv_dis->user = user;
    argv_dis->num = num;
    argv_dis->sign = &sign_menu;
    argv_dis->num_max = num_max;
    argv_dis->name_cur = name_cur;
    argv_dis->logout = &logout;
    argv_dis->messageboxsign = &messageboxsign;
    argv_dis->messageboxnum = &messageboxnum;
    argv_dis->change = &change;

    char a[50][USERNAME_SIZE];
    WindowInit();
    InitList(user);
    if (0 == access("date",0))
        Ser_LoadList(user);
    else
    {
        strcpy(a[0],"peter");
    strcpy(a[1],"ken");
    strcpy(a[2],"tom");
    AddUser(user,"andy","1",3,a);
    memset(a,0x0,sizeof(a));
    strcpy(a[0],"andy");
    strcpy(a[1],"ken");
    strcpy(a[2],"tom");
    AddUser(user,"peter","1",3,a);
    memset(a,0x0,sizeof(a));
    strcpy(a[0],"andy");
    strcpy(a[1],"peter");
    strcpy(a[2],"tom");
    AddUser(user,"ken","1",3,a);
    memset(a,0x0,sizeof(a));
    strcpy(a[0],"andy");
    strcpy(a[1],"ken");
    strcpy(a[2],"peter");
    AddUser(user,"tom","1",3,a);
    }
    pthread_mutex_init(&mut,NULL);
//    user->front = NULL;
//    user->next = NULL;
    fd_ = SerNetInit();
    keypad(stdscr,1);
    pthread_create(&pth_dis,NULL,Display,argv_dis);    //创建显示进程
//    sleep(2);
    pthread_create(&pth,NULL,Keyboard,argv_key);    //创建键盘控制进程
    sfd = create_and_bind(PORT_S);
    s = listen(sfd,SOMAXCONN);
    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&event);
    events = calloc(64,sizeof event);
    while(1)
    {
       int n = 0,i = 0;
       for (i = 0;i < n;i++)
       {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!events[i].events & EPOLLIN))
            {
                close(events[i].data.fd);
                continue;
            }
            else if (sfd == events[i].data.fd)    //新链接
            {
                struct sockaddr in_addr;
                socklen_t in_len;
                int infd = 0;
                char hbuf[NI_MAXHOST],sbuf[NI_MAXSERV];

                in_len = sizeof in_addr;
                infd = accept(sfd,&in_addr,&in_len);
                if (infd == -1)
                    break;
                s = getnameinfo(&in_addr,in_len,
                                hbuf,sizeof hbuf,
                                sbuf,sizeof sbuf,
                                NI_NUMERICHOST | NI_NUMERICSERV);
                if (s == 0)    //新链接，套接口描述符=infd，IP=hbuf，端口=sbuf。
                    s = make_socket_non_blocking(infd);
                if (s == -1)
                    abort();
                event.data.fd = infd;
                event.events = EPOLLIN | EPOLLET;
                s = epoll_ctl(efd,EPOLL_CTL_ADD,infd,&event);
                if (s == -1)
                    abort();
            }
            else   //有数据接收
            {
                ssize_t count = 0;
                char buf[DATELEN];
                memset(buf,0x0,sizeof buf);
                count = read(events[i].data.fd,buf,sizeof buf);
                if (count == -1)
                    break;
                else if (count == 0)   //客户端断开链接
                {
                    char name[USERNAME_SIZE];
                    GetName(user, events[i].data.fd, name);
                    close(events[i].data.fd); 
                    OnLine(user, name, 3);
                    break;
                }
                //////处理接收到的数据
                
            }
       }
    }
    free(events);
    close(sfd);
    return 0;
}

void *Display(void *argv1)
{
    struct arg_ser_dis *argv2= NULL;
    struct User_List *user = NULL,*cur = NULL;
    argv2 = (struct arg_ser_dis *)argv1;
    cur = argv2->user;
    int x = 0,y = 0;
    int x1 = 0,y1 = 0;
    char userlist[200][USERNAME_SIZE];
    char friendlist[200][USERNAME_SIZE];
    int online_sign[200] = {0};
    char friend_cur[USERNAME_SIZE];
    int sumoffriends = 0;
    int dis_temp = 0;
    int sumofonlineuser = 0;
    int sumofonlineuser_temp = 0;
    int sumofuser = 0;
    int sumofuser_temp = 0;
    
    clear();
    leaveok(stdscr,1);
    noecho();
    int a=0;
    while(1)
    {
        if (CLOSE == *argv2->logout)     //关闭并保存数据
        {
            Ser_SaveList(argv2->user);
            endwin();
            exit(0);
        }
        x1 = x;
        y1 = y;
        sumofuser_temp = sumofuser;
        sumofonlineuser_temp = sumofonlineuser;
        GetSize(&y,&x);
        if (x != x1 || y != y1)
            clear();   //清屏
        sumofuser = ListLength(argv2->user,&sumofonlineuser);
        if (1 == *argv2->change || sumofuser != sumofuser_temp || sumofonlineuser != sumofonlineuser_temp)
        {
            *argv2->change = 0;
            Ser_windows(&x,&y,sumofonlineuser,sumofuser);   //初始化窗口界
            memset(userlist,0x0,sizeof(userlist));
            memset(friendlist,0x0,sizeof(friendlist));
            argv2->num_max[0] = GetUserList(argv2->user,userlist);    //获取用户列表
            GetOnline(argv2->user,online_sign);   //获取用户在线状态
            Ser_DisplayUserList(x,y,userlist,argv2->num[0],argv2->num_max[0],online_sign,argv2->name_cur);    //显示用户列表
            argv2->num_max[1] = GetFriendList(argv2->user,argv2->name_cur,friendlist);    //获取对应用户好友列表
            Ser_DisplayFriendList(x,y,friendlist,argv2->num[1],argv2->num_max[1],friend_cur);   //显示好友列表 
            Ser_DisPlayMsg(x,y,argv2->user,argv2->name_cur,friend_cur);
        }
        usleep(10000);
    }
    pthread_exit(NULL);
}

void *Keyboard(void *argv1)
{
    struct arg_ser_key *argv;
    int a = 0;
    char s[2];
    argv = (struct arg_ser_key *)argv1;
    KeyboardControl(argv->num,argv->num_max,argv->sign,argv->logout,argv->message,argv->message_sign,&a,s,argv->messageboxsign,argv->messageboxnum,argv->change);
    pthread_exit(NULL);
}




