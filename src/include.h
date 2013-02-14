#ifndef MYQQ_H
#define MYQQ_H

#include "user.h"

//数据类型宏 - 字符串
#define DATETYPE_FRIENDSLIST       "1"    //好友列表
#define DATETYPE_COMMUNICATE       "2"    //通信信息
#define DATETYPE_FRIENDSINFO       "3"    //好友信息
#define DATETYPE_OFFLINEMESSAGE    "4"    //离线消息

#define LOGIN_SUCCESS              "5"    //登陆成功
#define LOGIN_PASSWORD_WRONG       "6"    //密码错误

#define CREATEUSER_SUCCESS         "7"    //注册用户成功

#define MENU_ADDFRIEND             "8"    //添加新好友
#define MENU_SENDMESSAGE           "9"    //发送消息
#define MENU_DELFRIEND             "10"   //删除好友
#define ADDFRIEND_SUCCESS          "11"   //添加好友成功

//消息归属宏
#define MSGOWN_MYSELF               "1"   //消息归属为自身
#define MSGOWN_FRIENDS              "2"   //消息归属为好友

//数据类型宏 - 整形
#define DATETYPE_FRIENDSLIST_I      1
#define DATETYPE_COMMUNICATE_I      2
#define DATETYPE_FRIENDSINFO_I      3
#define MENU_ADDFRIEND_I            8
#define MENU_SENDMESSAGE_I          9
#define MENU_DELFRIEND_I            10
#define ADDFRIEND_SUCCESS_I         11

#define MSGOWN_MYSELF_I             1
#define MSGOWN_FRIENDS_I            2

//客户端相关命令
#define CM_QUIT                      "quit"       //退出
#define CM_MENU_FRIENDLIST           "fl"     //选中好友列表
#define CM_ADDFRIEND                 "add "   //添加好友
#define CM_DELFRIEND                 "del "   //删除好友

#define CLOSE                       1
#define RUNNING                     0
#define MSG_OK                      0
#define MSG_CANCEL                  1

struct arg_dis    //显示线程参数结构体
{
    char *name;    //返回选中用户名
    char *name_loacl;
    int *sign;     //功能选项
    int *num;      //选中用户序列号
    struct Cli_Friendslist *friendslist;    //好友列表
    int *sumoffriends;   //好友个数
    char *addfriendsname;
    int *messageboxsign;   //提示窗口激活标识
    int *messageboxnum;    //提示窗口选中标识
    int *logout;      //退出标识
};

struct arg_recv   //网络接收线程参数结构体
{
    int *fd;     //套接口描述符
    char *sender;   //消息发送者
    char *message;  //接收消息缓冲区
    struct Cli_Friendslist *friendslist;  //好友列表
    int *sumoffriends;   //好友总数
    int *message_sign;  //消息缓冲区空标识
    int *messageboxsign;    //消息窗口激活标识
};

struct arg_key    //按键检测线程参数结构体
{
    int *sign;    //功能选项
    int *num;     //选中用户序列号
    int *num_max;   //列表最大数
    int *logout;    //退出标识
    char *message;    //接收缓冲区
    int *message_sign;   //缓冲区空标识，0表示为空
    int *addfriend_sign;   //添加/删除好友状态激活标识
    char *addfriendsname;   //添加/删除好友好友名缓冲区
    int *messageboxsign;   //提示窗口激活标识
    int *messageboxnum;    //提示窗口选中标识
};

struct arg_ser_newconnect   //处理新链接线程参数结构体
{
    struct User_List *user;    //用户链表
    char name[DATELEN];       //新链接的用户名
    char passwd[DATELEN];     //新链接的用户密码
    char message[DATELEN];    //消息缓冲区
    char temp[2][USERNAME_SIZE];
    int *fd;                  //链接的套接字描述符
    int *sumofcli;            //当前在线用户人数
};

struct arg_ser_key       //服务器按键检测线程参数结构体
{
    int *sign;     //功能选择
    int *num;      //选中对象序号
    int *num_max;     //列表最大对象数
    int *logout;      //退出标识
    char *message;     //消息缓冲区
    int *message_sign;     //缓冲区空标识，0表示为空
    int *messageboxsign;   //提示窗口激活标识
    int *messageboxnum;    //提示窗口选中标识
    int *change;
};

struct arg_ser_dis    //显示进程参数结构
{
    struct User_List *user;    //用户链表
    int *num;                 //被选中用户序号
    int *sign;               //功能选择标识
    int *num_max;           //各列表对象最大数
    char *name_cur;           //返回被选中用户名
    int *logout;              //退出标识
    int *messageboxsign;      //提示窗口激活标识
    int *messageboxnum;       //提示窗口选中标识
    int *change; 
};

#endif










