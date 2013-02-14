#include "include.h"
#include "net.h"
#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>

/*********************************
 *
 * 函数功能：服务器网络初始化
 * 返回值：成功 - 侦听套接字描述符
 *         socket失败 - 1
 *         bind失败 - 2
 *
 * ******************************/
int SerNetInit()
{
    int fp;
    struct sockaddr_in addr;
    if ((fp = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        perror("fp");
        return 1;
    }
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);
    int i = 1;
    setsockopt(fp,SOL_SOCKET,SO_REUSEADDR,&i,sizeof(i));
    if ((bind(fp,(struct sockaddr *)&addr,sizeof(addr))) < 0)
    {
//        perror("bind");
        return 2;
    }
    listen(fp,CLIENTNUM);
    return fp;
}

/****************************************
 *
 * 函数功能：客户端网络初始化
 * 返回值：成功 - 链接后的套接字描述符
 *         socket失败 - 1
 *         connect失败 - 2
 *
 ***************************************/
int CliNetInit()
{
    int fp;
    int i = 1;
    struct sockaddr_in addr;
    if ((fp = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
//        perror("fp");
        return 1;
    }
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    setsockopt(fp,SOL_SOCKET,SO_REUSEADDR,&i,sizeof(i));
    inet_pton(AF_INET,IP,&addr.sin_addr);
    if ((fp = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
//        perror("fp");
        return 1;
    }
    if (connect(fp,(struct sockaddr *)&addr,sizeof(addr)) < 0)
    {
//        perror("connect");
        return 2;
    }
    return fp;
}


/********************************
 *
 * 函数功能：接受一个链接
 * 返回值：成功 - 链接描述符
 *         失败 - 1
 *
 * ****************************/
int Accept(int fp,struct sockaddr_in cli_addr)
{
    int fp_;
    socklen_t len;
    len = 0;
    if ((fp_ = accept(fp,(struct sockaddr *)&cli_addr,&len)) < 1)
        return 1;
    return fp_;
}


/*********************************
 *
 * 函数功能：接收数据
 * 返回值：成功 - 接收到的字节数
 *         失败 - 1
 *
 ********************************/
int Recv(int fp,char *date)
{
    unsigned int num = 0;
    num = recv(fp,date,DATELEN*sizeof(char),0);
    return num*DATELEN;
}

/********************************
 *
 * 函数功能：发送数据
 * 返回值：成功 - 发送的字节数
 *         失败 - 1
 *
 ********************************/
int Send(int fp,char *date)
{
    unsigned int num = 0;
    char temp[DATELEN];
    memset(temp,0x0,sizeof(temp));
    strcpy(temp,date);
    num = send(fp,temp,DATELEN*sizeof(char),0);
    return num*DATELEN;
}


/****************************************
 *
 * 函数功能：服务器端接收客户端信息
 * 返回值：发送信息 - 0
 *         添加好友 - 1
 *         错误 - -1
 *
 * ************************************/
int RecvMessage(struct User_List *user,struct Friend *friends,char sender[USERNAME_SIZE],int fp,char buf[DATELEN],char receiver[USERNAME_SIZE])
{
    unsigned int num = 0;
    struct MessageLog *msglog = NULL;
    struct User_List *recv_user = user;
    char sign[2];
    memset(sign,0x0,sizeof(sign));
    memset(receiver,0x0,sizeof(receiver));
    memset(buf,0x0,sizeof(buf));
    if (recv(fp,sign,3*sizeof(char),0) <= 0)   //接收功能选择
        return -1;
    if (0 == strcmp(sign,MENU_ADDFRIEND))   //如果为1，则为添加好友，返回1
        return MENU_ADDFRIEND_I;
    if (0 == strcmp(sign,MENU_DELFRIEND))    //删除好友
        return MENU_DELFRIEND_I;
    if ((num = recv(fp,receiver,USERNAME_SIZE*sizeof(char),0)) < 0)     //接收信息接收人用户名
        return -1;
    if ((recv(fp,buf,DATELEN*sizeof(char),0)) < 0)     //接收信息内容
        return -1;
    while(NULL != recv_user)
    {
        if (0 == strcmp(recv_user->user.name,receiver))
            break;
        recv_user = recv_user->next;
    }
    InsertToMessagelog(&recv_user->user.friends,sender,buf,1);    //将信息写入发送者的聊天记录
    return MENU_SENDMESSAGE_I;
}

/*****************************************
 *
 * 函数功能：服务器端将信息传给接收者用户
 * 参数：Friends - 该用户的好友列表
 *       fp - 套接口描述符
 *       message = 消息结构体
 *       name - 接收消息的用户名
 * 返回值：成功 - 0
 *
 * *************************************/
int SendMessage(struct User_List *user,char message[DATELEN],char receiver[USERNAME_SIZE],char selfname[USERNAME_SIZE])
{
    unsigned int num = 0;
    int fd;
    if ((fd = GetSocket(user,receiver)) < 0)
        return -1;
//    send(fd,DATETYPE_COMMUNICATE,2,0);   //发送数据类型——发送消息
    Send(fd,DATETYPE_COMMUNICATE);
    usleep(SENDDELAYTIME);
    if ((num = send(fd,message,strlen(message),0)) > 0)
    {
        send(fd,selfname,USERNAME_SIZE*sizeof(char),0);
        return 0;
    }
    return -1;
}

/*******************************************
 *
 * 函数功能：发送并删除用户离线消息
 * 参数：user - 欲检测离线消息的用户节点
 * 返回值： 成功 - 返回离线消息数
 *          没有离线消息 - 0
 *
 * ****************************************/
int SendOffLineMessage(struct User_List *user)
{
    int fd;
    int num = 0;
    struct OffLineMessage *offline = &user->user.offlinemessage;
    struct OffLineMessage *del;
    fd = user->user.socket;
    if (NULL != user->user.offlinemessage.next)
    {
        offline = offline->next;
        while (NULL != user->user.offlinemessage.next)
        {
            offline = user->user.offlinemessage.next;
            num++;
//            offline = user->user.offlinemessage.next;
            del = offline;
            Send(fd,DATETYPE_COMMUNICATE);
//            send(fd,DATETYPE_COMMUNICATE,2,0);   //发送数据类型——发送消息
            usleep(SENDDELAYTIME);
            Send(fd,offline->message);
            usleep(SENDDELAYTIME);
            send(fd,offline->Sender,USERNAME_SIZE*sizeof(char),0);
            if (NULL != offline->next)
            {
                offline->front->next = offline->next;
                offline->next->front = offline->front;
            }
            else
                offline->front->next = NULL;
            free(del);
            user->user.sumofofflinemsg--;
        }
        return num;

    }
//    printf("没有离线消息\n");
    return 0;
}

/********************************
 *
 * 函数功能：发送好友列表
 *
 * *****************************/
int SendFriendlist(struct User_List *user,struct User_List *cur,char *name,int fd)
{
    int numoffriend = 0;
    char message[DATELEN];
    char FriendList[FRIENDS_MAX][USERNAME_SIZE];
    GetFriendList(user,name,FriendList);  //获取用户好友列表
    Send(fd,DATETYPE_FRIENDSLIST);   //发送信息类型——好友列表
    usleep(SENDDELAYTIME);
    memset(message,0x0,sizeof(message));
    sprintf(message,"%d",cur->user.numoffriend); 
    Send(fd,message); 
    usleep(SENDDELAYTIME); 
    numoffriend = 1;
    while(numoffriend <= cur->user.numoffriend)
    {
        Send(fd,FriendList[numoffriend]);
        char online[2]; 
        online[0] = OnLine(user,FriendList[numoffriend],1) + '0';
        online[1] = '\0';
        usleep(SENDDELAYTIME);    //发送后延时
        send(fd,online,sizeof(online),0);
        usleep(SENDDELAYTIME);
        numoffriend++;
    }
    return 0;
}

int create_and_bind(char *port)
{
    struct addrinfo hints;
    struct addrinfo *result,*rp;
    int s,sfd;

    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, port, &hints,&result);
    if (s != 0)
    {
        fprintf(stderr,"getaddinco:%s\n",gai_strerror(s));
        return -1;
    }
    for (rp = result;rp != NULL;rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
        if (sfd == -1)
            continue;
        s = bind(sfd,rp->ai_addr,rp->ai_addrlen);
        if (s == 0)
            break;
        close(sfd);
    }
    if (rp == NULL)
    {
        fprintf(stderr,"could not bind\n");
        return -1;
    }
    freeaddrinfo(result);
    return sfd;
}

int make_socket_non_blocking(int sfd)
{
    int flags,s;
    flags = fcntl(sfd,F_GETFL,0);
    if (flags == -1)
    {
        perror("fcntl");
        return -1;
    }
    flags |= O_NONBLOCK;
    s = fcntl(sfd,F_SETFL,flags);
    if (s == -1)
    {
        perror("fcntl");
        return -1;
    }
    return 0;
}


