#ifndef NET_H
#define NET_H

//#include "net.h"
#include "user.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5556      //端口
#define PORT_S "5556"
#define CLIENTNUM 100   //最大用户连接数
#define DATELEN 128*2     //单次数据长度
#define IP "192.168.1.7"     //服务器IP地址

struct package{
    uint32_t size;   //数据大小
    uint16_t type;   //数据类型
    xmlChar *message;   //数据内容
};

int SerNetInit(void);
int CliNetInit(void);
int Accept(int fp,struct sockaddr_in cli_addr);
int Recv(int fp,char *date);
int Send(int fp,char *date);
int RecvMessage(struct User_List *user,struct Friend *friends,char sender[USERNAME_SIZE],int fp,char buf[DATELEN],char receiver[USERNAME_SIZE]);
int SendMessage(struct User_List *user,char message[DATELEN],char receiver[USERNAME_SIZE],char selfname[USERNAME_SIZE]);
int SendOffLineMessage(struct User_List *user);
int SendFriendlist(struct User_List *user,struct User_List *cur,char *name,int fd);
int create_and_bind(char *port);
int make_socket_non_blocking(int sfd);

#endif
