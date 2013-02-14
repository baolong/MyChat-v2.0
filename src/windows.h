#ifndef WINDOW_H
#define WINDOW_H

#include "windows.h"
#include "user.h"
#include "net.h"

int Cli_Welcome();
int WindowInit();
int MessageBox(int x,int y,char *message,int length,int sign);
int Cli_Login();
int Cli_Apply(int x,int y,char name[USERNAME_SIZE],char passwd[USERNAME_SIZE]);
int ColorInit();
int GetSize(int *x,int *y);
int Cli_Windows(int *x,int *y);
int Ser_windows(int *x,int *y,int sumofonlineuser,int sumofuser);
int Cli_DisplayFriendList(int x,int y,struct Cli_Friendslist *friendslist,int num,int sum,char name[USERNAME_SIZE]);
int Cli_DisplayUserDate(int x,int y,struct User_List *user,char name[USERNAME_SIZE]);
int Ser_DisplayUserList(int x,int y,char list[][USERNAME_SIZE],int num,int sum,int sign[100],char name[USERNAME_SIZE]);
int Ser_DisplayFriendList(int x,int y,char friends[][USERNAME_SIZE],int num,int sum,char name[USERNAME_SIZE]);
int KeyboardControl(int *num,int *max_num,int *sign,int *logout,char *message,int *message_sign,int *addfriend_sign,char *addfriendsname,int *messageboxsign,int *messageboxnum,int *change);
int Ser_DisPlayMsg(int x,int y,struct User_List *user,char username[USERNAME_SIZE],char friendsname[USERNAME_SIZE]);
int Cli_DisPlayMsg(int x,int y,struct Cli_Friendslist *friendslist,char friendsname[USERNAME_SIZE]);

#endif
