#define _MY_FRIENDS_C

#include "my_friends.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include"/usr/local/mysql/include/mysql.h"
#include <sys/types.h>
#include <sys/socket.h>

#include "my_mysql.h"
#include "my_pack.h"
#include "my_err.h"


int add_fir(PACK *pack, MYSQL mysql1) 
{
    PACK            *recv_pack = pack;
    MYSQL           mysql = mysql1;
    MYSQL_RES       *result;
    MYSQL_ROW       row, row1;
    BOX             *tmp = box_head;
    int             ret;
    char            need[100];
    printf("add_friend()------\n");
    sprintf(need, "select * from user_data where account = %d", recv_pack->data.recv_account);   //客户端发过来的准备要添加的账号
    pthread_mutex_lock(&mutex);
    ret   =   mysql_query(&mysql, need);
  
    if (!ret) 
    {
        //printf("进来了....\n");
        result   =    mysql_store_result(&mysql);
        row      =    mysql_fetch_row(result);
        if (row == NULL) 
        {
            pthread_mutex_unlock(&mutex);
            return -1;
        }
        memset(need  ,  0  ,  sizeof(need));                                           //本人的账号
        sprintf(need,"select *from friends where user = %d and friends_user = %d", recv_pack->data.send_account, recv_pack->data.recv_account);
        ret = mysql_query(&mysql, need);
        if(ret) 
        {
            my_err("mysql_query",__LINE__);
            fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
            exit(1);
        }


        if (!ret)   
        { 
            //printf("这里可以吗？...可以\n");
            

            // result  =   mysql_store_result(&mysql);
            // row1    =   mysql_fetch_row(result);
            
            //printf("ssssssssssssssssssssssss\n");
            // if (row1 != NULL) 
            // {
            //     printf("这里面吗？\n");
            //     pthread_mutex_unlock(&mutex);
            //     return -1;
            // }
            
            memset(need, 0, sizeof(need)); //%d 和 %s 一定不能搞混了
            sprintf(need, "用户 %s (账号：%d)请求添加您为好友\n", recv_pack->data.send_user , recv_pack->data.send_account);
            
            //printf("准备进到if else 判断这里\n");
            if(atoi(row[3]) == 0)
            {
                printf("进到if\n");
                while (tmp) 
                {
                    //printf("会循环吗？");
                    if (tmp->recv_account == recv_pack->data.recv_account)     //客户端发过来的准备要添加的账号
                    {
                        break;
                    }
                    tmp = tmp->next;
                }
                //printf("出while循环\n");
                if(tmp == NULL) 
                {
                    //printf("进到if这里\n");  会进到这里
                    tmp = (BOX *)malloc(sizeof(BOX));
                    tmp->recv_account = recv_pack->data.recv_account;                         //准备加的好友
                    tmp->friend_number = 0;
                    tmp->talk_number = 0;
                    
                    strcpy(tmp->write_buff[tmp->friend_number], need);   //need, "账号为%d,昵称为%s的用户发来好友请求\n"
                    tmp->plz_account[tmp->friend_number++] = recv_pack->data.send_account;    //发送请求的账号
                    if (box_head == NULL)
                    {
                        box_head = box_tail = tmp;
                        box_tail->next = NULL;
                    } 
                    else 
                    {
                        box_tail->next = tmp;
                        box_tail = tmp;
                        box_tail->next = NULL;
                    }   
                }
                else   //tmp !=NULL
                {
                    printf("进到else 这里\n");
                    tmp->plz_account[tmp->friend_number] = recv_pack->data.send_account;      
                    strcpy(tmp->write_buff[tmp->friend_number], need);                        //need, "账号为%d,昵称为%s的用户发来好友请求\n"
                    tmp->friend_number++;
                } 
            
                pthread_mutex_unlock(&mutex);
                return 0;
            }
            else 
            {
                printf("进到else");
                recv_pack->data.send_fd = atoi(row[4]);
                strcpy(recv_pack->data.recv_user, row[1]);
                strcpy(recv_pack->data.read_buff, need);
                recv_pack->type = FRIENDS_PLZ;
                
                if (send(recv_pack->data.send_fd, recv_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                } 
                
                pthread_mutex_unlock(&mutex);
                return 0;
            }
        }
    }
}


int friends_plz(PACK *pack, MYSQL mysql1) 
{
    char            need[100]; 
    MYSQL           mysql = mysql1;
    PACK            *recv_pack = pack;
    
    pthread_mutex_lock(&mutex);

    sprintf(need, "insert into friends values(%d,%d,0)", recv_pack->data.send_account , recv_pack->data.recv_account);
    
    if(mysql_query(&mysql,need)) 
    {
        my_err("mysql_query",__LINE__);
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        exit(1);
    }

    memset(need, 0, sizeof(need));
    sprintf(need , "insert into friends values(%d,%d,0)", recv_pack->data.recv_account , recv_pack->data.send_account);
   
    if(mysql_query(&mysql,need)) 
    {
        my_err("mysql_query",__LINE__);
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        exit(1);
    }
    pthread_mutex_unlock(&mutex);

    return 0;
}   
     

int del_friend(PACK *pack, MYSQL mysql1) {
    PACK            *recv_pack  =  pack;
    MYSQL           mysql       =  mysql1;
    char            need[100];
    MYSQL_ROW       row;
    MYSQL_RES       *result;

    pthread_mutex_lock(&mutex);
    
    sprintf(need, "select *from friends where user = %d and friends_user = %d", recv_pack->data.send_account, recv_pack->data.recv_account);
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if (row == NULL) {
        pthread_mutex_unlock(&mutex);    
        return -1;
    }
    
    
    memset(need, 0, sizeof(need));
    sprintf(need, "delete from friends where user = %d and friends_user = %d", recv_pack->data.send_account, recv_pack->data.recv_account);
    
    mysql_query(&mysql, need);
    memset(need, 0, sizeof(need));
    sprintf(need, "delete from friends where user = %d and friends_user = %d", recv_pack->data.recv_account, recv_pack->data.send_account);
    
    mysql_query(&mysql, need);
    pthread_mutex_unlock(&mutex);
    
    return 0;
}


FRIEND *look_list(PACK *pack, MYSQL mysql1) 
{
    PACK            *recv_pack = pack;
    MYSQL           mysql = mysql1;
    char            need[100];
    MYSQL_RES       *result, *result1;
    MYSQL_ROW       row, row1;
    FRIEND          *list;

    list = (FRIEND *)malloc(sizeof(FRIEND));
    list->friend_number = 0;   //好友数
    
    pthread_mutex_lock(&mutex);                           //要查看好友列表
    
    sprintf(need, "select *from friends where user = %d", recv_pack->data.send_account);
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);

    // printf("atoi(row[1]) = %d\n",atoi(row[1]));
    // list->friend_account[list->friend_number++]   =   atoi(row[1]);      //friend_user账号
    
    while (row = mysql_fetch_row(result))     //账号    昵称     登录状态
    {
        //printf("atoi(row[1]) = %d\n",atoi(row[1]));
        list->friend_account[list->friend_number]   =   atoi(row[1]);      //friend_user账号

        memset(need , 0 , sizeof(need));
        sprintf(need , "select * from user_data where account = %d",atoi(row[1]));
        mysql_query(&mysql , need);
        result1 = mysql_store_result(&mysql);
        row1 = mysql_fetch_row(result1);

        strcpy(list->friend_nickname[list->friend_number], row1[1]);
        list->friend_state[list->friend_number++] = atoi(row1[3]);
    } 

    pthread_mutex_unlock(&mutex);
    return list;
    
}














 // else             
            // {
            //     printf("else部分\n");
            //     recv_pack->data.send_fd  =  atoi(row[4]);
            //     strcpy(recv_pack->data.recv_user , row[1]);    //row[1] = user_name
            //     strcpy(recv_pack->data.read_buff , need);      //need, "账号为%d,昵称为%s的用户发来好友请求\n"
            //     recv_pack->type = FRIENDS_PLZ;                 //好友请求
            //     pthread_mutex_lock(&mutex);
            //     memset(need , 0 , sizeof(need));
            //     sprintf(need, "insert into friends values(%d,%d,0)", recv_pack->data.send_account , recv_pack->data.recv_account);
            //     //mysql_query(&mysql , need);
            //     if(mysql_query(&mysql,need)) 
            //     {
            //         my_err("mysql_query",__LINE__);
            //         fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
            //         exit(1);
            //     }

            //     memset(need, 0, sizeof(need));
            //     sprintf(need , "insert into friends values(%d,%d,0)", recv_pack->data.recv_account , recv_pack->data.send_account);
            //     //mysql_query(&mysql , need);
            //     if(mysql_query(&mysql,need)) 
            //     {
            //         my_err("mysql_query",__LINE__);
            //         fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
            //         exit(1);
            //     }
            //     pthread_mutex_unlock(&mutex);
            //     if (send(recv_pack->data.send_fd, recv_pack, sizeof(PACK), 0) < 0) 
            //     {
            //         my_err("send", __LINE__);
            //     } 
            //     pthread_mutex_unlock(&mutex);
            //     return 0;
            // }