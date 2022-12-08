#ifndef _MY_PACK_H
#define _MY_PACK_H
#include <pthread.h>
#include "/usr/local/mysql/include/mysql.h"

#define MAX_FRIEND 500
#define   MAXIN           1024
#define   EXIT            -1
#define   ACCOUNT_ERROR   -2
#define   OK               0

#define   L                 1           // 登录           

#define   R                 2           // 注册 

#define   FIND_PASSWORD     3           // 找回密码

#define   CHANGE_PASSWORD   4           // 修改密码

#define   ADD_FRIEND        5           // 添加好友            

#define   DEL_FRIEND        6           // 删除好友

#define   LOOK_LIST         7           // 查看好友列表

#define   SEND_FMES         8           // 发消息

#define   CREATE_GROUP      9           // 创建群

#define   ADD_GROUP         10          // 加入群 

#define   EXIT_GROUP        11          // 退出群

#define   DEL_MEMBER        12          // 删除群成员

#define   S_A               13          // 设置管理员

#define   DEL_ADMIN         14          // 删除管理员

#define   SEND_GMES         15          // 发送群消息

#define   LOOK_GROUP        16          // 查看加入的群

#define   DIS_GROUP         17          // 群主删除群

#define   SEND_FILE          18          // 发送文件

#define   RECV_FILE         19

#define   FRIENDS_PLZ       20          // 好友请求

#define   RECV_FMES         24

#define   READ_MESSAGE       25       

#define   READ_GMES          26

#define   RECV_GMES          27

#define   LOOK_MEMBER        29        //查看群成员

#define   LOOK_GROUP_LIST    30        //查看已加入的群列表

#define   READ_FILE            21

#define   OK_FILE            22



/**
 *  -----------------------锁-------------------------------- 
 */

pthread_mutex_t mutex;
pthread_mutex_t mutex_cli;
pthread_cond_t  cond_cli;
pthread_cond_t  cond;

/**
 *------------------------结构体----------------------------- 
 */

typedef struct {
    int               cont;
    int               send_fd;           //套接字
    int               recv_fd;
    int               send_account;      //账号
    int               recv_account;
    char              send_user[50];     
    char              recv_user[50];     
    char              read_buff[MAXIN];
    char              write_buff[MAXIN];
} DATA;

typedef struct {
    int               type;
    DATA              data;
} PACK;



typedef struct {
    int               friend_number;             //数量
    int               friend_account[500];       //账号
    char              friend_nickname[500][20];  //昵称
    int               friend_state[500];         //状态
} FRIEND;

typedef struct {
    int               number;
    char              message[500][MAXIN];
    int               send_user[500];     //发送消息的人
    int               recv_user[500];     //接受消息的人
} MESSAGE;

/**
 * --------------------------group--------------------------------
 */

typedef struct {
    int               group_account;
    int               message_number;
    int               send_account[500];
    char              message[500][MAXIN];
} GROUP_MESSAGE;

typedef struct {
    int               group_account;
    int               group_member_number;
    int               group_member_account[500];	
    char              group_member_nickname[500][20];
    int               group_member_state[500];
} GROUP;


typedef struct {
    int               group_account[100];
    char              group_name[100][20];
    int               number;
    int               group_state[100];
} GROUP_G;

/**
 *-------------------------file-------------------------------- 
 */

typedef struct {
    char              filename[50];
    int               send_account; 	
    char              send_nickname[50];
    int               have;
} FILE1;

// 消息盒子
typedef struct BOX {
    int               recv_account;             // 接受消息的人的账号
    int               send_account[500];        // 发送消息人的账号
    int               plz_account[500];         // 发送好友请求人的账号
    char              read_buff[500][MAXIN];    // 发送的消息
    char              write_buff[500][100];     // 发送的请求
    int               talk_number;              // 消息数量
    int               friend_number;            // 请求数量
    int               send_account1[500];       // 群里发送消息的人
    char              message[500][MAXIN];      // 消息内容
    int               number;                   // 群消息数量
    int               group_account[500];       // 群号
    struct BOX        *next;
} BOX;


BOX *box_head;
BOX *box_tail;

/**
 * ---------------------login---------------------- 
 */

int login(PACK *pack, MYSQL mysql1);

int registered(PACK *pack, MYSQL mysql1);

int change_password(PACK *pack, MYSQL mysql1);

int find_password(PACK *pack, MYSQL mysql1);


/**
 *--------------------friend---------------------
 */

// int add_fir(PACK *pack, MYSQL mysql1);

// int del_friend(PACK *pack, MYSQL mysql1);

// int friends_plz(PACK *pack, MYSQL mysql1);

// FRIEND *look_list(PACK *pack, MYSQL mysql1);

/**
 *---------------------message---------------------
 */

int send_message(PACK *pack, MYSQL mysql1);

int read_message(PACK *pack, MYSQL mysql1);

int del_message(PACK *pack, MYSQL msyql1);

int send_gmes(PACK *pack, MYSQL mysql1);

// int send_file(PACK *pack, MYSQL mysql1);
// int read_file(PACK *pack, MYSQL mysql1);

int ok_file(PACK *pack, MYSQL msyql1); 

/**
 *----------------------group--------------------- 
 */



int create_group(PACK *pack, MYSQL mysql1);

int add_group(PACK *pack, MYSQL mysql1);

int exit_group(PACK *pack, MYSQL mysql1);

int set_admin(PACK *pack, MYSQL mysql1);

int del_admin(PACK *pack, MYSQL mysql1);

int look_member(PACK *pack, MYSQL mysql1);

int look_group_list(PACK *pack, MYSQL mysql1);

int del_member(PACK *pack, MYSQL mysql1);





#endif
