#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include"/usr/local/mysql/include/mysql.h"
// #include "my_friends.h
 #include "my_pack.h"

#define MAXEPOLL  1024
#define PORT      10000
#define IP        "127.0.0.1"

void * deal(void* recv_pack);

int login(PACK * pack , MYSQL mysql);
int registered(PACK * pack , MYSQL mysql1);
int change_passwd(PACK * pack , MYSQL mysql1);
int find_password(PACK *pack, MYSQL mysql1);
int add_friend(PACK *pack , MYSQL mysql1);
int friend_plz(PACK *pack , MYSQL mysql1);
int del_friend(PACK *pack ,MYSQL mysql1);
FRIEND * look_list(PACK * pack , MYSQL mysql1);

void my_err(char *err_string , int line)
{
    fprintf(stdout , "line : %d\n",line);
    perror(err_string);
    exit(1);
}

int main()
{
    int                        i;
    int                        socklen;
    int                        sock_fd;
    int                        conn_fd;
    int                        curfds;
    int                        kdpfd;
    int                        nfds;
    int                        acceptcont = 0;
    int                        size;
    char                       need[MAXEPOLL];
    MYSQL                      mysql;
    MYSQL_RES                  *result;
    MYSQL_ROW                  row;
    PACK                       recv_pack;          //接收数据
    PACK                       *pack;              //??
    pthread_t                  pid;
    struct sockaddr_in         cli;
    struct sockaddr_in         s_addr;
    struct epoll_event         ev;
    struct epoll_event         events[MAXEPOLL];

    mysql_init(&mysql);
    if (mysql_library_init(0, NULL, NULL) != 0)      // 初始化数据库
    {
        my_err("mysqllibrary_init", __LINE__);
    }  
    // if (sock == mysql_real_connect(&mysql, "127.0.0.1", "root", NULL , "happy", 0, NULL, 0))  //连接数据库
    // {
    //     my_err("mysql_real_connect", __LINE__);
    // }
    if (!(mysql_real_connect(&mysql,"127.0.0.1","root",NULL,"happy",0,NULL,0))) 
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("mysql_real_connect");
        exit(1);
    }
    if (mysql_set_character_set(&mysql, "utf8") < 0) //设置中文字符集
    {
        my_err("mysql_set_character_set", __LINE__);
    }
    pthread_mutex_init(&mutex, NULL);
    socklen = sizeof(struct sockaddr_in);
    sock_fd = socket(AF_INET , SOCK_STREAM , 0);
    if(sock_fd == -1)
    {
        my_err("socket",__LINE__);
    }    
    memset(&s_addr , 0 , sizeof(s_addr));
    s_addr.sin_family        =     AF_INET;
    s_addr.sin_addr.s_addr   =     htonl(INADDR_ANY);
    s_addr.sin_port          =     htons(PORT);

    //setsockopt(sock_fd , SOL_SOCKET , SO_REUSEADDR , &curfds , sizeof(curfds));
    if((bind(sock_fd , (struct sockaddr*)&s_addr , sizeof(s_addr))) < 0)
    {
        my_err("bind",__LINE__);
    }    
    if((listen(sock_fd , 64)) < 0)
    {
        my_err("listen",__LINE__);
    }

    kdpfd   =   epoll_create(1);
    if(kdpfd == -1)
    {
        my_err("epoll_creat" , __LINE__);
    }
    ev.events     =     EPOLLIN | EPOLLET;
    //ev.events     =     EPOLLIN ;
    ev.data.fd    =     sock_fd;           //这两条信息存储到树上的节点中
    size          =     sizeof(events)/sizeof(events[0]);
    //printf("109、、、、、、、、、、、、、\n");
    if(epoll_ctl(kdpfd , EPOLL_CTL_ADD , sock_fd , &ev) == -1)
    {
        my_err("epoll_ctl",__LINE__);
    }
    curfds = 1;
    while(1)
    {   
        nfds = epoll_wait(kdpfd , events , size , -1) ;  
        printf("nfds = %d curfds = %d\n",nfds , curfds);                            
        if(nfds < 0)   //nfds：已经就绪的文件描述符数量
        {
            my_err("epoll_wait",__LINE__);
        }
        for(i = 0 ; i < nfds ; i++)      
        {
            if(events[i].data.fd == sock_fd)      //用于监听的套接字
            {
                if((conn_fd = accept(events[i].data.fd , (struct sockaddr *)&cli , &socklen )) < 0)
                {
                    my_err("accept",__LINE__);
                }
                printf("连接成功！套接字conn_fd = %d\n",conn_fd);
                acceptcont++; //连接数++
                                                          
                ev.events = EPOLLIN | EPOLLET;
                //ev.events  = EPOLLIN;
                ev.data.fd = conn_fd;   
                
                if((epoll_ctl(kdpfd , EPOLL_CTL_ADD , conn_fd , &ev)) < 0)
                {
                    my_err("epoll_ctl",__LINE__);
                }
                curfds++;
                continue;
            }
            else if(events[i].events & EPOLLIN)        //events[i].events:要检测的事件,
            {
                memset(&recv_pack , 0 , sizeof(PACK));     
                //在上面经过accept创建了一个conn_fd 套接字传送数据
                if((recv(events[i].data.fd , &recv_pack , sizeof(PACK) , MSG_WAITALL ))< 0)  //第一个参数的内容传递到第二个
                {
                    close(events[i].data.fd);
                    perror("recv");
                    continue;
                }
                if (recv_pack.type == EXIT) 
                {
                    if ((send(events[i].data.fd, &recv_pack, sizeof(PACK), 0)) < 0)
                    {
                        my_err("send", __LINE__);
                    }
                    memset(need, 0, sizeof(need));
                    sprintf(need, "update user_data set user_state = 0 where user_state = 1 and socket = %d", events[i].data.fd);
                    mysql_query(&mysql, need);
                    epoll_ctl(kdpfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    curfds--;  
                    continue;
                }
			    if (recv_pack.type == LOGIN) 
                {
 		    	    memset(need, 0, sizeof(need));
                    //printf("line 171: recv_pack.data.send_account = %d\n",recv_pack.data.send_account);
                    sprintf(need, "select *from user_data where account = %d", recv_pack.data.send_account);
                    pthread_mutex_lock(&mutex);
                    //mysql_query(&mysql, need);
                    if(mysql_query(&mysql,need)) 
                    {
                        fprintf(stderr,"176 Query failed (%s)\n",mysql_error(&mysql));
                        exit(1);
                    }
                    result = mysql_store_result(&mysql);
                    row = mysql_fetch_row(result);
                    if(row == NULL)
                    {
                        my_err("mysql_fetch_row",__LINE__);
                    }
                    if (!row) 
                    {
                        recv_pack.type = ACCOUNT_ERROR;
                        memset(recv_pack.data.write_buff, 0, sizeof(recv_pack.data.write_buff));
                        printf("$$sad\n");
                        strcpy(recv_pack.data.write_buff, "passwd error");
                        if ((send(events[i].data.fd, &recv_pack, sizeof(PACK), 0)) < 0) 
                        {
                             my_err("send", __LINE__);
                        }                     
                        pthread_mutex_unlock(&mutex);
                        continue;
                    }
                    memset(need , 0 , sizeof(need));
                    printf("line 195: recv_pack.data.send_account = %d\n",recv_pack.data.send_account);
                    sprintf(need , "update user_data set socket = %d where account = %d",events[i].data.fd , recv_pack.data.send_account);
                    mysql_query(&mysql , need);
                    pthread_mutex_unlock(&mutex);
                }
                recv_pack.data.recv_fd   =    events[i].data.fd;
 			    pack                     =    (PACK*)malloc(sizeof(PACK));
 			    memcpy(pack , &recv_pack , sizeof(PACK));
 			    pthread_create(&pid, NULL, deal, (void*)pack);
            }
        }   
    }
}


void * deal(void* recv_pack)
{
    PACK         *pack ;
    pack = (PACK*)recv_pack;
    MYSQL        mysql;
    BOX          *tmp = box_head;       //消息盒子
    pthread_detach(pthread_self());
    mysql_init(&mysql);
    // if (NULL == mysql_init(&mysql)) 
    // {
    //     my_err("mysql_init", __LINE__);
	// }
    if ((mysql_library_init(0, NULL, NULL))!= 0)      // 初始化数据库
    {
        my_err("mysqllibrary_init", __LINE__);
    }  
    // if (mysql_real_connect(&mysql, "127.0.0.1", "root", NULL , "happy", 0, NULL, 0))  //连接数据库
    // {
    //     my_err("mysql_real_connect", __LINE__);
    // }
    if (!(mysql_real_connect(&mysql,"127.0.0.1","root",NULL,"happy",0,NULL,0))) 
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
        perror("mysql_real_connect");
        exit(1);
    }
    if ((mysql_set_character_set(&mysql, "utf8")) < 0) //设置中文字符集
    {
        my_err("mysql_set_character_set", __LINE__);
    }

    switch(pack->type)
    {
        case LOGIN:
        {   
            if((login(pack , mysql)) != 0)
            {
                pack->type = ACCOUNT_ERROR;
                memset(pack->data.write_buff , 0 , sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff , "passwd error");
                if(send(pack->data.recv_fd , pack , sizeof(PACK) , 0) < 0)
                {
                    my_err("send",__LINE__);
                }
            }
            else
            {
                memset(pack->data.write_buff , 0 , sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff , "good");
                if(send(pack->data.recv_fd , pack , sizeof(PACK) , 0) < 0)
                {
                    my_err("send",__LINE__);
                }
                while(tmp != NULL)
                {
                    if(tmp->recv_account == pack->data.send_account)        //找到相匹配的退出
                    {
                        break;
                    }
                    tmp = tmp->next;
                }
                if (tmp == NULL) 
                {
                    tmp                =   (BOX *)malloc(sizeof(BOX));
                    tmp->recv_account  =   pack->data.send_account;
                    tmp->talk_number   =   tmp->friend_number = 0;
                    tmp->number        =   0;
                    tmp->next          =   NULL;
                    if (box_head == NULL) 
                    {
                        box_head = box_tail = tmp;
                    }
                    else
                    {
                        box_tail->next = tmp;
                        box_tail = tmp;
                    }
                    if (send(pack->data.recv_fd, tmp, sizeof(BOX), 0) < 0) 
                    {
                        my_err("send", __LINE__);
                    }
                } 
                else 
                {
                    if (send(pack->data.recv_fd, tmp, sizeof(BOX), 0) < 0) 
                    {
                        my_err("send", __LINE__);
                    }
                    tmp->friend_number   =    0;
                    tmp->talk_number     =    0;
                }
            }
            break;
        }

        case REGISTERED:
        {
            registered(pack, mysql);
            memset(pack->data.write_buff , 0 , sizeof(pack->data.write_buff));
            strcpy(pack->data.write_buff , "registered sucess");
            if((send(pack->data.recv_fd , pack , sizeof(PACK) , 0)) < 0)
            {
                my_err("send" , __LINE__);
            }
            break;
        }

        case CHANGE_PASSWORD:
        {
            if(change_passwd(pack , mysql) == 0)
            {
                memset(pack->data.write_buff , 0 , sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff , "success");
                if((send(pack->data.recv_fd , pack , sizeof(PACK) , 0)) < 0)
                {
                    my_err("send" , __LINE__);
                }
            }
            else
            {
                memset(pack->data.write_buff , 0 , sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff,"fail");
                if(send(pack->data.recv_fd , pack , sizeof(PACK) , 0) < 0)
                {
                    my_err("send" , __LINE__);
                }
            }
            break;
        }
        
        case ADD_FRIEND :
        {
            if(add_friend(pack , mysql) == 0)
            {
                pack->type = ADD_FRIEND;
                memset(pack->data.write_buff , 0 , sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff , "success");
                if(send(pack->data.recv_fd , pack , sizeof(PACK) , 0) < 0)
                {
                    my_err("send" , __LINE__);
                }
            }
            else
            {
                memset(pack->data.write_buff , 0 , sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff , "fail");
                if(send(pack->data.recv_fd , pack , sizeof(PACK) , 0) < 0)
                {
                    my_err("send" , __LINE__);
                }
            }
            break;
        }
        case FRIENDS_PLZ :
        {
            friend_plz(pack , mysql);
            break;
        }
        case DEL_FRIEND:
        {
            if(del_friend(pack , mysql) == 0)
            {
                memset(pack->data.write_buff , 0 , sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff , "success");
                if(send(pack->data.send_fd , pack , sizeof(PACK) , 0) < 0)
                {
                    my_err("send",__LINE__);
                }
            }
            break;
        }
        case LOOK_LIST:  //查看好友列表
        {
            FRIEND *list = look_list(pack , mysql);
            if(list->friend_number != 0)
            {
                memset(pack->data.write_buff,0,sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff,"success");
                if(send(pack->data.send_fd , pack , sizeof(PACK) , 0) < 0)
                {
                    my_err("send",__LINE__);
                }
            }
            else
            {
                memset(pack->data.write_buff,0,sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff , "fail");
                if(send(pack->data.send_fd,pack,sizeof(PACK),0) < 0)
                {
                    my_err("send",__LINE__);
                }
            }
            break;
        }

       

            

    }
}

FRIEND * look_list(PACK * pack , MYSQL mysql1)
{
    MYSQL  mysql = mysql1;
    MYSQL_ROW    row,row1;
    MYSQL_RES    *result,*result1;
    PACK         *recv_pack = pack;
    char         need[100];
    FRIEND       *list;
    list = (FRIEND *)malloc(sizeof(FRIEND));
    list->friend_number = 0;
    pthread_mutex_lock(&mutex);
    sprintf(need,"select *from friends where user = %d",recv_pack->data.send_account);
    //mysql_query(&mysql, need);
    
    if(mysql_query(&mysql,need)) 
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        exit(1);
    }
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    
    while (row) 
    {
        list->friend_account[list->friend_number] = atoi(row[1]);
        memset(need, 0, sizeof(need));
        sprintf(need, "select *from user_data where account = %d", atoi(row[1]));
        
        mysql_query(&mysql, need);
        result1 = mysql_store_result(&mysql);
        row1 = mysql_fetch_row(result1);
        
        strcpy(list->friend_nickname[list->friend_number], row1[1]);
        list->friend_state[list->friend_number++] = atoi(row1[3]);
    }
}

int del_friend(PACK *pack ,MYSQL mysql1)
{
    MYSQL        mysql = mysql1;
    MYSQL_ROW    row;
    MYSQL_RES    *result;
    PACK         *recv_pack = pack;
    char         need[100];

    pthread_mutex_lock(&mutex);
    sprintf(need,"select *from friends where user = %d and friend_user = %d",recv_pack->data.send_account,recv_pack->data.recv_account);
    mysql_query(&mysql , need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(row == NULL)
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    memset(need , 0 , sizeof(need));
    sprintf(need,"delete from friends where user=%d and friend_user=%d",recv_pack->data.send_account,recv_pack->data.recv_account);
    if(mysql_query(&mysql,need)) 
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        exit(1);
    }
    memset(need , 0 , sizeof(need));
    sprintf(need,"delete from friends where user = %d and friend_user = %d",recv_pack->data.recv_account,recv_pack->data.send_account);
    if(mysql_query(&mysql,need)) 
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        exit(1);
    }
    pthread_mutex_unlock(&mutex);
    return 0 ;
}

int friend_plz(PACK *pack , MYSQL mysql1)
{
    char need[100];
    MYSQL mysql = mysql1;
    PACK *recv_pack = pack;

    pthread_mutex_lock(&mutex);
    memset(need , 0  , sizeof(need));
    sprintf(need , "insert into friends values(%d,%d,0)",recv_pack->data.recv_account , recv_pack->data.send_account);
    if(mysql_query(&mysql,need)) 
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        exit(1);
    }
    pthread_mutex_unlock(&mutex);
    return 0; 
}

int add_friend(PACK *pack , MYSQL mysql1)
{
    MYSQL       mysql = mysql1;
    MYSQL_ROW   row , row1;
    MYSQL_RES   *result = NULL;
    PACK        *recv_pack = pack;
    BOX         *tmp = box_head;
    int         ret;
    char        need[100] = {};

    sprintf(need , "select *from user_data where account = %d",recv_pack->data.recv_account);
    pthread_mutex_lock(&mutex);
    ret = mysql_query(&mysql , need);
    if(!ret)
    {
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        if(row == NULL)
        {
            pthread_mutex_unlock(&mutex);
            return -1;
        }
        memset(need , 0 , sizeof(need));
        sprintf(need , "select * from friends where user = %d and friend_user = %d",recv_pack->data.send_account,recv_pack->data.recv_account);
        ret = mysql_query(&mysql , need);
        if(!ret)
        {
            result = mysql_store_result(&mysql);
            row1 = mysql_fetch_row(result);
            if(row1 != NULL)
            {
                pthread_mutex_unlock(&mutex);
                return -1;
            } 
            memset(need , 0 , sizeof(need));
            sprintf(need , "账号为%d,%s用户发来好友请求\n",recv_pack->data.send_account , recv_pack->data.send_user);
            if(atoi(row[3]) == 0)
            {
                while(tmp)
                {
                    if(tmp->recv_account == recv_pack->data.recv_account)
                        break;
                    tmp=tmp->next;
                }
                if(tmp != NULL)
                {
                    tmp->plz_account[tmp->friend_number] = recv_pack->data.send_account;
                    strcpy(tmp->write_buff[tmp->friend_number] , need);
                    tmp->friend_number++;
                }
                else
                {
                    tmp = (BOX *)malloc(sizeof(BOX));
                    tmp->recv_account - recv_pack->data.recv_account;
                    tmp->friend_number = 0;
                    tmp->talk_number = 0;
                    strcpy(tmp->write_buff[tmp->friend_number] , need);
                    tmp->plz_account[tmp->friend_number++] = recv_pack->data.send_account;
                    if(box_head == NULL)
                    {
                        box_head = box_tail = tmp;
                        box_tail = NULL;
                    }
                    else
                    {
                        box_tail->next = tmp;
                        box_tail = tmp;
                        box_tail->next = NULL;
                    }
                    
                }
                pthread_mutex_unlock(&mutex);
                return 0;
            }
            else
            {
                recv_pack->data.send_fd = atoi(row[4]);
                strcpy(recv_pack->data.recv_user , row[1]);
                strcpy(recv_pack->data.read_buff , need);
                recv_pack->type = FRIENDS_PLZ;
                if(send(recv_pack->data.send_fd , recv_pack , sizeof(PACK) , 0) < 0)
                    my_err("send",__LINE__);
                 pthread_mutex_unlock(&mutex);
                 return 0;
            }
        }
    }
}


int login(PACK * pack , MYSQL mysqll)
{
    int            ret;
    int            i;
    char           need[100];
    PACK           *recv_pack = pack;
    MYSQL          mysql = mysqll;
    MYSQL_RES      *result;
    MYSQL_ROW      row;

    memset(need , 0 , sizeof(need));
    sprintf(need , "select * from user_data where account = %d",recv_pack->data.recv_account);
    pthread_mutex_lock(&mutex);
    ret = mysql_query(&mysql,need);
    if(ret) 
    {
        fprintf(stderr,"line 605: Query failed (%s)\n",mysql_error(&mysql));
        my_err("mysql_query",__LINE__);
    }
    if(!ret)
    {
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);      //读取每一行的数据
        if(row == NULL)
        {
            pthread_mutex_unlock(&mutex);
            return -1;
        }
        if((strcmp(row[2] , recv_pack->data.read_buff)) == 0)
        {
            strcpy(recv_pack->data.send_user , row[1]);
            recv_pack->data.recv_fd = atoi(row[4]);
            memset(need , 0 , sizeof(need));
            sprintf(need , "update user_data set user_state = 1 where account = %d",pack->data.send_account);
            if(mysql_query(&mysql,need)) 
            {
                fprintf(stderr,"624 Query failed (%s)\n",mysql_error(&mysql));
                my_err("mysql_query",__LINE__); 
            }
            mysql_free_result(result);
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        else
        {
            recv_pack->data.recv_fd = atoi(row[4]);
            pthread_mutex_unlock(&mutex);
            return -1;
        }
    }
    else
    {
        printf("query fail!\n");
        pthread_mutex_unlock(&mutex);
        return -1;
    }
}

int registered(PACK * pack , MYSQL mysql1)
{
    FILE     *fp;
    char     need[100] = {};
    PACK     *recv_pack = pack;
    MYSQL    mysql = mysql1;
    int      user_number = 100;

    pthread_mutex_lock(&mutex);
    if((fp = fopen("user_number.txt" , "r"))==NULL)
    {
        my_err("fopen",__LINE__);
    }
    user_number++;
    fread(&user_number , sizeof(int) , 1 , fp);
    memset(need , 0 , sizeof(need));
    //recv_pack->data.send_account = user_number-1;
    sprintf(need , "insert into user_data value(%d,\"%s\",\"%s\",%d,%d)",user_number, recv_pack->data.send_user , recv_pack->data.read_buff , 0 , recv_pack->data.recv_fd);
    recv_pack->data.send_account = user_number-1;
    if(mysql_query(&mysql,need)) 
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        exit(1);
    }
    fclose(fp);
    if((fp = fopen("user_number.txt" , "w")) == NULL)
    {
        printf("打开文件失败！\n");
        exit(-1);
    }   
    fwrite(&user_number , sizeof(int) , 1 , fp);
    fclose(fp);
    pthread_mutex_unlock(&mutex);
    return 0;
}

int change_passwd(PACK * pack , MYSQL mysql1)
{
    MYSQL          mysql = mysql1;
    MYSQL_RES      *result = NULL;  
    MYSQL_ROW      row;
    PACK           *recv_pack = pack;
    char need[100] = {};

    sprintf(need , "select * from user_data where account = %d",recv_pack->data.send_account);
    pthread_mutex_lock(&mutex);
    mysql_query(&mysql , need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(row)
    {
        if(strcmp(recv_pack->data.read_buff , row[2]) == 0)  //read_buff ：输入的密码
        {
            recv_pack->data.recv_fd = atoi(row[4]);    //套接字
            memset(need , 0 , sizeof(need));
            sprintf(need , "update user_data set passwd = \"%s\" where account = %d\n",recv_pack->data.write_buff ,  recv_pack->data.send_account);
            mysql_query(&mysql , need);
            pthread_mutex_unlock(&mutex);

            return 0;
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            return -1;
        }
        
    }
    else
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
}

int find_password(PACK *pack, MYSQL mysql1) 
{
    MYSQL           mysql = mysql1;
    MYSQL_RES       *result;
    MYSQL_ROW       row;
    char            need[150];
    PACK            *recv_pack = pack;
    int             cont = 0;

    pthread_mutex_lock(&mutex);
    sprintf(need, "select *from friends where user = %d", recv_pack->data.send_account);
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    while (row = mysql_fetch_row(result)) 
    {
        cont++;
    }
    if (cont != recv_pack->data.recv_account) 
    {
        strcpy(recv_pack->data.write_buff, "密码无法找回");
        memset(need, 0, sizeof(need));
        sprintf(need, "select * from user_data where account = %d", recv_pack->data.send_account);
        mysql_query(&mysql, need);
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        if (!row) 
        {
            strcpy(recv_pack->data.write_buff, "账号错误");
        }
        if (send(recv_pack->data.recv_fd, recv_pack, sizeof(PACK), 0) < 0) 
        {
            my_err("send", __LINE__); 
        }   
    } 
    else 
    {
        memset(need, 0, sizeof(need));
        sprintf(need, "select * from user_data where account = %d", recv_pack->data.send_account);
        mysql_query(&mysql, need);
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        if (!row) 
        {
            strcpy(recv_pack->data.write_buff, "账号错误");
        }
        else 
        {
            sprintf(recv_pack->data.write_buff, "你的密码是%s", row[2]);
        }
        if (send(recv_pack->data.recv_fd, recv_pack, sizeof(PACK), 0) < 0) 
        {
            my_err("send", __LINE__);
        }
    }
    
    pthread_mutex_unlock(&mutex);
    return 0;
}