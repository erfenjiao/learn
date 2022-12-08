#include"/usr/local/mysql/include/mysql.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "my_mysql.h"
#include "my_pack.h"
#include "my_friends.h"
#include "my_err.h"

#define MAXEPOLL 1024


void *deal(void *recv_pack);
int ok_file(PACK *pack, MYSQL mysql1);


int main() {
	 int                        i;
	 int                        sock_fd;
	 int                        conn_fd;
	 int                        socklen;
	 int                        acceptcont = 0;
	 int                        kdpfd;
	 int                        curfds;
	 int                        nfds;
	 char                       need[MAXIN];
	 MYSQL                      mysql;
	 struct sockaddr_in         cli;
     struct sockaddr_in         s_addr;
	 struct epoll_event         ev;
	 struct epoll_event         events[MAXEPOLL];
	 PACK                       recv_pack;
	 PACK                       *pack;
	 pthread_t                  pid;
     MYSQL_RES                  *result;
     
    
     pthread_mutex_init(&mutex, NULL);
	 socklen = sizeof(struct sockaddr_in);
	 mysql = accept_mysql();
	 sock_fd = socket(AF_INET , SOCK_STREAM , 0);
    if(sock_fd == -1)
    {
        my_err("socket",__LINE__);
    }    
    memset(&s_addr , 0 , sizeof(s_addr));
    s_addr.sin_family        =     AF_INET;
    s_addr.sin_addr.s_addr   =     htonl(INADDR_ANY);
    s_addr.sin_port          =     htons(10000);

    //setsockopt(sock_fd , SOL_SOCKET , SO_REUSEADDR , &curfds , sizeof(curfds));
    if((bind(sock_fd , (struct sockaddr*)&s_addr , sizeof(s_addr))) < 0)
    {
        my_err("bind",__LINE__);
    }    
    if((listen(sock_fd , 64)) < 0)
    {
        my_err("listen",__LINE__);
    }

	 kdpfd = epoll_create(MAXEPOLL);

	 ev.events = EPOLLIN | EPOLLET;
	 ev.data.fd = sock_fd;

	 if(epoll_ctl(kdpfd, EPOLL_CTL_ADD, sock_fd, &ev) < 0) {
	 	my_err("epoll_ctl", __LINE__);
	 }

	 curfds = 1;

	while(1) {
	 	if((nfds = epoll_wait(kdpfd, events, curfds, -1)) < 0){
	 		my_err("epoll_wait", __LINE__);
	    }

 	    for (i = 0; i < nfds; i++) 
        { 
		    if (events[i].data.fd == sock_fd) 
            {
 			    if ((conn_fd = accept(sock_fd, (struct sockaddr*)&cli, &socklen)) < 0) 
                {
 				    my_err("accept", __LINE__);
 			    }
 			    printf("连接成功,套接字编号%d\n", conn_fd);
 			    acceptcont++;

 			    ev.events = EPOLLIN | EPOLLET;
 			    ev.data.fd = conn_fd;

 			    if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, conn_fd, &ev) < 0) 
                {
 				    my_err("epoll_ctl", __LINE__);
 			    }
 			    curfds++;
 			    continue;
 		    } 
            else if (events[i].events & EPOLLIN) 
            { 
			    memset(&recv_pack, 0, sizeof(PACK));
 			    if (recv(events[i].data.fd, &recv_pack, sizeof(PACK), MSG_WAITALL) < 0) 
                {
 				    close(events[i].data.fd);
 				    perror("recv");
 				    continue;
 			    }
                if (recv_pack.type == EXIT) 
                {
                    if (send(events[i].data.fd, &recv_pack, sizeof(PACK), 0) < 0) 
                    {
                        my_err("send", __LINE__);
                    }
                    memset(need, 0, sizeof(need));
                    sprintf(need, "update user_data set user_state = 0 where user_state = 1 and user_socket = %d", events[i].data.fd);
                    mysql_query(&mysql, need);
                    epoll_ctl(kdpfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    curfds--;
                
                    continue;
                }
			    if (recv_pack.type == L) 
                {
 		    	    memset(need  ,  0  ,  sizeof(need));
                    sprintf(need  , "select *from user_data where account = %d", recv_pack.data.send_account);
                    pthread_mutex_lock(&mutex);
                    mysql_query(&mysql, need);
                    result = mysql_store_result(&mysql);
                    if (!mysql_fetch_row(result)) 
                    {
                        recv_pack.type = ACCOUNT_ERROR;
                        memset(recv_pack.data.write_buff, 0, sizeof(recv_pack.data.write_buff));
                        printf("$$sad\n");
                        strcpy(recv_pack.data.write_buff, "password error");
                        if (send(events[i].data.fd, &recv_pack, sizeof(PACK), 0) < 0) 
                        {
                            my_err("send", __LINE__);
                        }
                        pthread_mutex_unlock(&mutex);
                        continue;
                    }
 		    	    memset(need, 0, sizeof(need)); 
 		    	    sprintf(need  , "update user_data set user_socket = %d where account = %d", events[i].data.fd, recv_pack.data.send_account);
 	    		    mysql_query(&mysql  , need); 
                    pthread_mutex_unlock(&mutex);
                }
                recv_pack.data.recv_fd   =   events[i].data.fd;
 			    pack   =   (PACK*)malloc(sizeof(PACK));
 			    memcpy(pack  ,  &recv_pack  ,  sizeof(PACK));           //收包，从客户端传来的信息
 			    pthread_create(&pid  ,  NULL  ,  deal  ,  (void*)pack);    //创建线程处理这个信息
            }
        }
    } 
}



/*************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 */

void *deal(void *recv_pack) 
{
    pthread_detach(pthread_self());
	PACK               *pack;
	int                i;
    BOX                *tmp = box_head;
	MYSQL              mysql;
	mysql = accept_mysql();
	pack = (PACK*)recv_pack;
	switch(pack->type)
	{
		case L:
		{
			if (login(pack, mysql) != 0) 
            {
                //printf("登录失败！\n");
                pack->type = ACCOUNT_ERROR;
                memset(pack->data.write_buff  ,  0  ,  sizeof(pack->data.write_buff));
				strcpy(pack->data.write_buff     ,     "password error");
                if(send(pack->data.recv_fd  ,  pack  ,  sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
			} 
                else 
                {
                    memset(pack->data.write_buff  ,  0  ,  sizeof(pack->data.write_buff));
                    strcpy(pack->data.write_buff     ,     "good");
                    int ret;
                    if((ret = send(pack->data.recv_fd, pack, sizeof(PACK), 0)) < 0) 
                    {
                        my_err("send", __LINE__);
                    }
                    while(tmp != NULL) 
                    {
                        if(tmp->recv_account == pack->data.send_account)         //登陆之后找到本账号的消息盒子
                        {
                            break;
                        }
                        tmp = tmp->next;
                    }
                    if(tmp == NULL)               //如果盒子为空，还需要初始化这个盒子才能发送
                    {
                        tmp                  =     (BOX *)malloc(sizeof(BOX));
                        tmp->recv_account    =     pack->data.send_account;
                        tmp->talk_number     =     tmp->friend_number = 0;
                        tmp->number          =     0;
                        tmp->next            =     NULL;
                        if (box_head == NULL) 
                        {
                            box_head = box_tail = tmp;
                        } 
                        else 
                        {
                            box_tail->next    =     tmp;
                            box_tail          =     tmp;
                        }
                        if (send(pack->data.recv_fd, tmp, sizeof(BOX), 0) < 0)  //把盒子发送出去
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
                        tmp->friend_number    =   0;
                        tmp->talk_number      =   0;
                    }
                }
                break;
			}
        case R:
            {
                registered(pack, mysql);
                memset(pack->data.write_buff  ,  0  ,  sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff     ,     "registered success!");
                if (send(pack->data.recv_fd  ,  pack  ,  sizeof(PACK), 0) < 0)   //把创建成功的消息发送出去
                {
                    my_err("send", __LINE__);
                }
                break;
            }
        case CHANGE_PASSWORD:
            {
                if (change_password(pack, mysql) == 0) 
                {
                    memset(pack->data.write_buff  ,  0  ,  sizeof(pack->data.write_buff));
                    strcpy(pack->data.write_buff     ,     "success");              //把创建成功的消息发送出去
                    if (send(pack->data.recv_fd, pack   ,  sizeof(PACK), 0) < 0) 
                    {
                        my_err("send", __LINE__);
                    }
                } 
                else 
                {
                    memset(pack->data.write_buff  ,  0  ,  sizeof(pack->data.write_buff));
                    strcpy(pack->data.write_buff     ,     "fail");
                    if (send(pack->data.recv_fd  ,  pack  ,sizeof(PACK), 0) < 0) 
                    {
                        my_err("send", __LINE__);        
                    }
                }
                break;
            }
/**
 *----------------------------friends----------------------------------------------- 
 */
        case ADD_FRIEND:
        {
            if (add_fir(pack, mysql) == 0) 
            {
                pack->type = ADD_FRIEND;
                memset(pack->data.write_buff  ,  0  ,  sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff     ,     "success");

                if (send(pack->data.recv_fd  ,  pack  ,sizeof(PACK) , 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            } 
            else 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "fail");

                if (send(pack->data.recv_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);        
                } 
            }
                break;
            }
        case FRIENDS_PLZ:
        {
            friends_plz(pack, mysql);
            break;
        }
        case DEL_FRIEND:          
        {
            if (del_friend(pack, mysql) == 0) 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "success");
                    
                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            } 
            else 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "fail");
                
                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            }
            break;
        }
        case LOOK_LIST:
        {
            FRIEND *list = look_list(pack, mysql);
            if (list->friend_number != 0) 
            {
                memset(pack->data.write_buff  ,  0  ,  sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff     ,     "success");
                if (send(pack->data.send_fd  , pack  , sizeof(PACK)   ,  0) < 0)       //将得到的数据发送到客户端
                {
                    my_err("send", __LINE__);
                }
                if (send(pack->data.send_fd  , list  , sizeof(FRIEND) ,  0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            } 
            else 
            {
                memset(pack->data.write_buff  ,  0  ,  sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff     ,     "fail");
                if (send(pack->data.send_fd  , pack ,  sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
                if (send(pack->data.send_fd  , list ,  sizeof(FRIEND), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            }
            break;       
        }

         case SEND_FMES:
        {
            if (send_message(pack, mysql) == -1) 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "fail");

                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            } 
            else 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "succss");

                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            }
            break;
        }

 /****************************************** group *********************************************/       
       
        case CREATE_GROUP:
        {
            if (create_group(pack, mysql) == 0) 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "success");

                printf("creat_group() :pack->data.write_buff = %s\n", pack->data.write_buff);

                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            } 
            else 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "fail");

                printf("creat_group() :pack->data.write_buff = %s\n", pack->data.write_buff);

                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                } 
            }
        break; 
        }
        
        
        case ADD_GROUP:
        {
            if (add_group(pack, mysql) == 0) 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "success");
                    
                printf("%s\n", pack->data.write_buff);
                    
                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            } 
            else 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "fail");
                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                } 
            }
            break;  
        }

    
        
        case EXIT_GROUP:
        {
            if (exit_group(pack, mysql) == 0) 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "success");
                
                //printf("%s\n", pack->data.write_buff);
                
                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0)
                 {
                    my_err("send", __LINE__);
                }
            }
            
            else 
             {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "fail");

                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                } 
            }

            break; 

        }

       
        case SEND_GMES:
        {
            if (send_gmes(pack, mysql) == -1) 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "fail");

                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            } 
            else 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "succss");

                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            }
            break;
        }
        case READ_MESSAGE:
        {
            read_message(pack, mysql);
            break;
        }
   

        case S_A:    //设置管理员
        {
            if (set_admin(pack, mysql) == 0) 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "success");
                
                printf("%s\n", pack->data.write_buff);
                
                send(pack->data.send_fd, pack, sizeof(PACK), 0);
                
            } 
            else 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "fail");
                
                send(pack->data.send_fd, pack, sizeof(PACK), 0) ;
                   
            }
            break; 
        }
        case DEL_ADMIN:
        {
            if (del_admin(pack, mysql) == 0) 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "success");
                
                printf("%s\n", pack->data.write_buff);
               
                send(pack->data.send_fd, pack, sizeof(PACK), 0);
                  
            } 
            else 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "fail");
                
                send(pack->data.send_fd, pack, sizeof(PACK), 0);
                
            }
            break; 
        }
        case LOOK_MEMBER:
        {
            look_member(pack, mysql);
            break;
        }
        case LOOK_GROUP_LIST:
        {
            look_group_list(pack, mysql);
            break;
        }
        case FIND_PASSWORD:
        {
            find_password(pack, mysql);
            break;
        }
        case DEL_MEMBER:
        {
            if (del_member(pack, mysql) == 0) 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "success");
                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
            } 
            else 
            {
                memset(pack->data.write_buff, 0, sizeof(pack->data.write_buff));
                strcpy(pack->data.write_buff, "fail");
                if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                } 
            }
            break;   
        }
        case SEND_FILE:
        {
            
            pthread_mutex_lock(&mutex);
            int fd = open("2", O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IXUSR);
            write(fd, pack->data.read_buff, 1023);
            close(fd);
            printf("%d\n", pack->data.cont);
            if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
            {
                my_err("send", __LINE__);
            }
            pthread_mutex_unlock(&mutex);
            break;
        }
        case READ_FILE:
        {
            printf("READ_FILE: 能到这里来吗?\n");
            //pthread_mutex_lock(&mutex);
            int fd = open("2", O_RDONLY);
            if(fd == -1)
            {
                my_err("open",__LINE__);
            }
            printf("open after\n");
            if(lseek(fd, 0, SEEK_SET) < 0)
            {
                my_err("lseek" , __LINE__);
            }
            printf("lseek after\n");
            memset(pack->data.read_buff, 0, sizeof(pack->data.read_buff));

            printf("READ_FILE ： read 前\n");
            while (read(fd, pack->data.read_buff, 1023) > 0) 
            {
                strcpy(pack->data.write_buff, "ok");
                
                break;
            }
            
            
            printf("read 后");

            if (send(pack->data.send_fd, pack, sizeof(PACK), 0) < 0) 
            {
                my_err("send", __LINE__);
            }
            close(fd);
            //pthread_mutex_unlock(&mutex);
            break;
        }
         case OK_FILE:
        {
            printf("service : sssssss^^\n");
            ok_file(pack, mysql);
            break;
        }
       
	}
	close_mysql(mysql);
}

int ok_file(PACK *pack, MYSQL mysql1) 
{
    MYSQL           mysql = mysql1;
    PACK            *recv_pack = pack;
    MYSQL_RES       *result;
    MYSQL_ROW       row;
    char            need[100];
    
    pthread_mutex_lock(&mutex);
    sprintf(need, "select *from user_data where account = %d", recv_pack->data.recv_account);
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if (!row) 
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    
    recv_pack->data.recv_fd = atoi(row[4]);
    recv_pack->type = RECV_FILE; 
    //printf("RECV_FILE  send  before\n");
    if (send(recv_pack->data.recv_fd, recv_pack, sizeof(PACK), 0) < 0) 
    {
        my_err("send", __LINE__);
    }
    //printf("RECV_FILE  send  after\n");
    recv_pack->type = OK_FILE;

    //printf("OK_FILE : send before\n");
    if (send(recv_pack->data.send_fd, recv_pack, sizeof(PACK), 0) < 0) 
    {
        my_err("send", __LINE__);
    }
    pthread_mutex_unlock(&mutex);
    printf("OK_FILE : send after\n");
    
    return 0;
}



/*************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 */


int create_group(PACK *pack, MYSQL mysql1) 
{
    MYSQL           mysql = mysql1;
    char            need[200];
    PACK            *recv_pack = pack;
    FILE            *fp;
    int             group_number;

    pthread_mutex_lock(&mutex);     //加锁

    //取出群号
    fp = fopen("group_number.txt", "r");
    fread(&group_number, sizeof(int), 1, fp);
    fclose(fp);                       //groups      （group_account   group_name）
    sprintf(need, "insert into groups values(%d,\"%s\",1)", group_number++, recv_pack->data.recv_user);
    mysql_query(&mysql, need);
    recv_pack->data.recv_account = group_number - 1;
    //printf("群号：%d",recv_pack->data.recv_account);
    fp = fopen("group_number.txt", "w");
    fwrite(&group_number, sizeof(int), 1, fp);
    fclose(fp);


    memset(need, 0, sizeof(need)); //group_members (group_account  group_name  user_account  user_name)
    sprintf(need, "insert into group_members values(%d,\"%s\",%d,\"%s\",1)", recv_pack->data.recv_account, recv_pack->data.recv_user, recv_pack->data.send_account, recv_pack->data.send_user);
    mysql_query(&mysql, need);


    pthread_mutex_unlock(&mutex);  //解锁

    return 0;
}

int add_group(PACK *pack, MYSQL mysql1) 
{
    MYSQL           mysql = mysql1;
    MYSQL_RES       *result;
    MYSQL_ROW       row;
    char            need[200];
    PACK            *recv_pack = pack;
    int             num;

    pthread_mutex_lock(&mutex);           //想要加进去的聊天室的号码
    sprintf(need, "select *from groups where group_account = %d", recv_pack->data.recv_account);
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if (!row) 
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    
    strcpy(recv_pack->data.recv_user, row[1]);
    num = atoi(row[2]);
    
    memset(need, 0, sizeof(need));                                      //要加入这个群的用户的账号 
    sprintf(need, "select *from group_members where group_account = %d and group_member_account = %d", recv_pack->data.recv_account, recv_pack->data.send_account);
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if (row)
     {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    
    memset(need, 0, sizeof(need));
    sprintf(need, "insert into group_members values(%d,\"%s\",%d,\"%s\",0)", recv_pack->data.recv_account, recv_pack->data.recv_user, recv_pack->data.send_account, recv_pack->data.send_user);
    mysql_query(&mysql, need);
    
    memset(need, 0, sizeof(need));
    sprintf(need, "update groups set group_state = %d where group_account = %d", num+1, recv_pack->data.recv_account);
    mysql_query(&mysql, need);
    
    pthread_mutex_unlock(&mutex);

    return 0;
}

int exit_group(PACK *pack, MYSQL mysql1) 
{
    MYSQL           mysql = mysql1;
    PACK            *recv_pack = pack;
    MYSQL_RES       *result;
    MYSQL_ROW       row;
    char            need[100];
    int             num;

    pthread_mutex_lock(&mutex);
    sprintf(need, "select *from groups where group_account = %d", recv_pack->data.recv_account);
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if (!row) 
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    
    num = atoi(row[2]);
    
    memset(need, 0, sizeof(need));
    sprintf(need, "select *from group_members where group_account = %d and group_member_account = %d", recv_pack->data.recv_account, recv_pack->data.send_account);
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    
    memset(need, 0, sizeof(need));
    sprintf(need, "delete from group_members where group_account = %d and group_member_account = %d", recv_pack->data.recv_account, recv_pack->data.send_account);
    
    mysql_query(&mysql, need);
    
    memset(need, 0, sizeof(need));
    sprintf(need, "update groups set group_mumber_number = %d where group_account = %d", num-1, recv_pack->data.recv_account);
    mysql_query(&mysql, need);

    pthread_mutex_unlock(&mutex);
    
    return 0;
}

int set_admin(PACK *pack, MYSQL mysql1) 
{
    MYSQL           mysql = mysql1;
    PACK            *recv_pack = pack;
    MYSQL_RES       *result;
    MYSQL_ROW       row;
    char            need[200];
    int             account;

    pthread_mutex_lock(&mutex);      //recv_account : 要被设置为管理员的账号   send_account 群主
    account = atoi(recv_pack->data.read_buff);   // group_account :去哪好
    sprintf(need, "select *from group_members where group_account = %d and group_member_account = %d and group_state = 1", recv_pack->data.recv_account, recv_pack->data.send_account);
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);

    
    memset(need, 0, sizeof(need));
    sprintf(need, "select *from group_members where group_account = %d and group_member_account = %d", recv_pack->data.recv_account, account);
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    
    memset(need, 0, sizeof(need));
    sprintf(need, "update group_members set group_state = 2 where group_account = %d and group_member_account = %d", recv_pack->data.recv_account, account);
    mysql_query(&mysql, need);
    pthread_mutex_unlock(&mutex);

    return 0;
}

int del_admin(PACK *pack, MYSQL mysql1) 
{
    MYSQL           mysql = mysql1;
    PACK            *recv_pack = pack;
    MYSQL_RES       *result;
    MYSQL_ROW       row;
    int             account;
    char            need[150];
    
    account = atoi(recv_pack->data.read_buff);
    pthread_mutex_lock(&mutex);
    sprintf(need, "select *from group_members where group_account = %d and group_member_account = %d and group_state = 1", recv_pack->data.recv_account, recv_pack->data.send_account);
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);

    memset(need, 0, sizeof(need));
    sprintf(need, "select *from group_members where group_account = %d and group_member_account = %d", recv_pack->data.recv_account, account);
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);

    memset(need, 0, sizeof(need));
    sprintf(need, "update group_members set group_state = 0 where group_account = %d and group_member_account = %d", recv_pack->data.recv_account, account);
    mysql_query(&mysql, need);
    pthread_mutex_unlock(&mutex);

    return 0;
}

int look_member(PACK *pack, MYSQL mysql1) 
{
    PACK            *recv_pack = pack;
    MYSQL           mysql = mysql1;
    MYSQL_RES       *result;
    MYSQL_ROW       row;
    char            need[100];
    GROUP           *group;

    pthread_mutex_lock(&mutex);
    group = (GROUP *)malloc(sizeof(GROUP));
    group->group_member_number = 0;
    sprintf(need, "select *from group_members where group_account = %d", recv_pack->data.recv_account);
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    
    while (row= mysql_fetch_row(result))
     {
        group->group_member_account[group->group_member_number] = atoi(row[2]);

        strcpy(group->group_member_nickname[group->group_member_number], row[3]);

        group->group_member_state[group->group_member_number++] = atoi(row[4]);
    }

    send(recv_pack->data.send_fd, recv_pack, sizeof(PACK), 0);

    send(recv_pack->data.send_fd, group, sizeof(GROUP), 0);

    // if (send(recv_pack->data.send_fd, recv_pack, sizeof(PACK), 0) < 0) {
    //     my_err("send", __LINE__);
    // }
    // if (send(recv_pack->data.send_fd, group, sizeof(GROUP), 0) < 0) {
    //     my_err("send", __LINE__);
    // }
    pthread_mutex_unlock(&mutex);

    return 0;
}

int look_group_list(PACK *pack, MYSQL mysql1) {
    PACK            *recv_pack = pack;
    MYSQL           mysql = mysql1;
    MYSQL_RES       *result;
    MYSQL_ROW       row;
    char            need[200];
    GROUP_G         *group;
    
    group = (GROUP_G *)malloc(sizeof(GROUP_G));
    
    group->number = 0;
    
    pthread_mutex_lock(&mutex);
    sprintf(need, "select *from group_members where group_member_account = %d", recv_pack->data.send_account);
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    
    while ((row = mysql_fetch_row(result))) 
    {
        group->group_account[group->number] = atoi(row[0]);
        strcpy(group->group_name[group->number], row[1]);
        group->group_state[group->number++] = atoi(row[4]);
    }


    if (send(recv_pack->data.send_fd, recv_pack, sizeof(PACK), 0) < 0) 
    {
        my_err("send", __LINE__);
    }
    if (send(recv_pack->data.send_fd, group, sizeof(GROUP_G), 0) < 0)
    {
        my_err("send", __LINE__);
    }

    pthread_mutex_unlock(&mutex);

    return 0;
}

int del_member(PACK *pack, MYSQL mysql1) 
{
    PACK            *recv_pack = pack;
    MYSQL           mysql = mysql1;
    MYSQL_RES       *result;
    MYSQL_ROW       row;
    char            need[200];
    int             account;

    account = atoi(recv_pack->data.read_buff);
    pthread_mutex_lock(&mutex);
    sprintf(need, "select *from group_members where group_account = %d and group_member_account = %d", recv_pack->data.recv_account, recv_pack->data.send_account);
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if (!row || atoi(row[4]) == 0) 
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    memset(need, 0, sizeof(need));
    sprintf(need, "delete from group_members where group_account = %d and group_member_account = %d", recv_pack->data.recv_account, account);
    mysql_query(&mysql, need);

    pthread_mutex_unlock(&mutex);
    return 0;
}



/*************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 talk*/



int send_message(PACK *pack, MYSQL mysql1) 
{
    char            need[100];
    MYSQL           mysql = mysql1;
    PACK            *recv_pack = pack;
    MYSQL_RES       *result;
    MYSQL_ROW       row;
    BOX             *tmp;

    pthread_mutex_lock(&mutex); 
    // user_data (account user_name passwd user_state user_socket)
    //先从user_account数据库中查到人,主要是找到这个人当前的登录状态 user_state
    sprintf(need, "select *from user_data where account = %d", recv_pack->data.recv_account);  
    
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);       
    row = mysql_fetch_row(result);
    
    
    //user_data(user_name) 
    strcpy(recv_pack->data.recv_user, row[1]);  
    

    //找到了我想要发送消息的人的登录状态，根据这个登录状态来判断将消息直接发送还是存储到chat_message这个数据库中
    if ((atoi(row[3]) == 1))  
    {
        //printf("send_message:此时处于登录状态\n");
        recv_pack->type = RECV_FMES;
        recv_pack->data.recv_fd = atoi(row[4]);

        memset(need, 0, sizeof(need));
        //在friends数据库中查找是否有想要与之聊天的账号
        sprintf(need, "select *from friends where user = %d", recv_pack->data.recv_account);
        mysql_query(&mysql, need);
        
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);

        while (row) 
        {
            if (atoi(row[1]) != recv_pack->data.send_account)  //friend_user
            {
                continue;   //与他相互加了好友的可能有多个，你只是其中一个
            }

            printf("\n我是：                             %s\n",recv_pack->data.send_user);
            printf("我想要聊天的对象是：              %s\n",recv_pack->data.recv_user);
            printf("我想给他发送的消息是(read_buff):  %s\n",recv_pack->data.read_buff);
                
            if (send(recv_pack->data.recv_fd, recv_pack, sizeof(PACK), 0) < 0) 
            {
                my_err("send", __LINE__);
            }
            memset(need, 0, sizeof(need));    //chat_messages (sned_user , recv_user ,  message , message_exit )
            sprintf(need, "insert into chat_messages values(%d,%d,\"%s\",1)", recv_pack->data.send_account, recv_pack->data.recv_account, recv_pack->data.read_buff);
            mysql_query(&mysql, need);
            break;            
        }
       
    } 
    else     //储存在 chat_messages中
    {
        printf("离线状态！\n");
        tmp = box_head;
        while (tmp != NULL) 
        {
            if (tmp->recv_account = recv_pack->data.recv_account) 
            {
                printf("我是：%s\n",recv_pack->data.send_user);
                printf("我想要聊天的对象是：%s\n",recv_pack->data.recv_user);
                printf("我想给他发送的消息是(read_buff):%s\n",recv_pack->data.read_buff);
                
                //什么账号，发送来什么信息
                tmp->send_account[tmp->talk_number] = recv_pack->data.send_account;
                strcpy(tmp->read_buff[tmp->talk_number++], recv_pack->data.read_buff);
                 
                //同样的信息给数据库chat_messages 一份
                memset(need, 0, sizeof(need));
                sprintf(need, "insert into chat_messages values(%d,%d,\"%s\",1)", recv_pack->data.send_account, recv_pack->data.recv_account,recv_pack->data.read_buff);
                mysql_query(&mysql, need);
                
                break;
            }
            tmp = tmp->next;
        }
    }
    pthread_mutex_unlock(&mutex);
    recv_pack->type = SEND_FMES;

    return 0;
}

int read_message(PACK *pack, MYSQL mysql1) 
{
    PACK            *recv_pack = pack;
    MYSQL           mysql =  mysql1;
    MYSQL_RES       *result;
    MYSQL_ROW       row;
    char            need[200];
    MESSAGE         *message;

    pthread_mutex_lock(&mutex);
    message = (MESSAGE *)malloc(sizeof(MESSAGE));
    message->number = 0;

    sprintf(need, "select *from chat_messages where send_user = %d and recv_user = %d and message_exit = 1 union select *from chat_messages where send_user = %d and recv_user = %d and message_exit = 1", recv_pack->data.send_account, recv_pack->data.recv_account, recv_pack->data.recv_account, recv_pack->data.send_account);
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);

    while (row) 
    {
       message->send_user[message->number] = atoi(row[0]);
       message->recv_user[message->number] = atoi(row[1]);
       strcpy(message->message[message->number++], row[2]);
    }

    if (send(recv_pack->data.send_fd, recv_pack, sizeof(PACK), 0) < 0) 
    {
        my_err("send", __LINE__);
    }

    if (send(recv_pack->data.send_fd, message, sizeof(MESSAGE), 0) < 0) {
        my_err("send", __LINE__);
    }
    
    pthread_mutex_unlock(&mutex);
    return 0;
}



int send_gmes(PACK *pack, MYSQL mysql1) 
{
    MYSQL           mysql = mysql1;
    PACK            *recv_pack = pack;
    MYSQL_RES       *result, *result1;
    MYSQL_ROW       row, row1;
    char            need[100];
    BOX             *tmp = box_head;

    pthread_mutex_lock(&mutex);
    recv_pack->type = RECV_GMES;
    sprintf(need, "select *from groups where group_account = %d", recv_pack->data.recv_account);
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    
    memset(need, 0, sizeof(need));
    sprintf(need, "select *from group_members where group_account = %d", recv_pack->data.recv_account);
    
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    while (row = mysql_fetch_row(result))   //循环发送
    {
        if (atoi(row[2]) == recv_pack->data.send_account) 
        {
            continue;
        }
        memset(need, 0, sizeof(need));
        sprintf(need, "select *from user_data where account = %d", atoi(row[2]));
       
        mysql_query(&mysql, need);
        result1 = mysql_store_result(&mysql);
        row1 = mysql_fetch_row(result1);
        
        if (atoi(row1[3]) == 1) 
        {
            if (send(atoi(row1[4]), recv_pack, sizeof(PACK), 0) < 0) 
            {
                my_err("send", __LINE__);
            }
        } 
        else 
        {
            while (tmp != NULL)
            {
                if (tmp->recv_account == atoi(row1[0])) 
                {
                    break;
                }
                tmp = tmp->next;
            }
          
            if (tmp == NULL)   //创建
            {
                tmp = (BOX *)malloc(sizeof(BOX));
                tmp->number = 0;
                tmp->recv_account = atoi(row1[0]);
                
                tmp->send_account1[tmp->number] = recv_pack->data.send_account;
                tmp->group_account[tmp->number] = recv_pack->data.recv_account;
                
                strcpy(tmp->message[tmp->number], recv_pack->data.read_buff);
                tmp->number++;
                
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

            else 
            {
                tmp->send_account1[tmp->number] = recv_pack->data.send_account;
                tmp->group_account[tmp->number] = recv_pack->data.send_account;
                
                strcpy(tmp->message[tmp->number], recv_pack->data.read_buff);

                tmp->number++;
            }
        }
    }
     
    recv_pack->type = SEND_GMES;
    pthread_mutex_unlock(&mutex);

    return 0;
}






/*************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 *************************************************************************************************************
 login*/








int login(PACK *pack, MYSQL mysql1) 
{
    int                i;
    int                ret;
    int                cont           =   0;
    char               inedx[100];	
    PACK               *recv_pack     =   pack;
    MYSQL              mysql          =   mysql1;
    MYSQL_RES          *result        =   NULL;
    MYSQL_ROW          row;

    sprintf(inedx, "select *from user_data where account = %d", recv_pack->data.send_account);
    pthread_mutex_lock(&mutex);	
    ret = mysql_query(&mysql, inedx);
    if(ret)
    {
        printf("query fail\n");
        pthread_mutex_unlock(&mutex);
        
        return -1;
    }    


    if (!ret) 
    {
        result  = mysql_store_result(&mysql);
        row     = mysql_fetch_row(result);
        if (row == NULL) 
        {
            pthread_mutex_unlock(&mutex);
            return -1;
        }
        if (strcmp(row[2], recv_pack->data.read_buff) == 0) 
        {
            strcpy(recv_pack->data.send_user, row[1]);
            memset(inedx, 0, sizeof(inedx));
            pack->data.recv_fd = atoi(row[4]);
            
            sprintf(inedx, "update user_data set user_state = 1 where account = %d", pack->data.send_account);
            mysql_query(&mysql, inedx);
            mysql_free_result(result);
            
            pthread_mutex_unlock(&mutex);
                
            return 0;
        } 
        else 
        {      
            pack->data.recv_fd = atoi(row[4]);
            pthread_mutex_unlock(&mutex);
            return -1;
        }
    } 
}

int registered(PACK *pack, MYSQL mysql1) 
{
    FILE               *fp;
    MYSQL              mysql = mysql1;	
    char               need[100];
    PACK               *recv_pack = pack;
    int                user_number;
	
    pthread_mutex_lock(&mutex);	        //加锁
    
    
    if ((fp = fopen("user_number.txt", "r")) == NULL) 
    {
        printf("打开文件失败\n");
        exit(-1);
    }
    fread(&user_number, sizeof(int), 1, fp);
	/**
     *  数据库操作 
     */
    sprintf(need, "insert into user_data values(%d,\"%s\",\"%s\",%d,%d)", user_number++, recv_pack->data.send_user, recv_pack->data.read_buff, 0, recv_pack->data.recv_fd);
    recv_pack->data.send_account = user_number-1;
    mysql_query(&mysql, need);
    
    
    fclose(fp);
    if ((fp = fopen("user_number.txt", "w")) == NULL) 
    {
        printf("打开文件失败\n");
        exit(-1);
    }
    fwrite(&user_number, sizeof(int), 1, fp);
    fclose(fp);

    pthread_mutex_unlock(&mutex);        //解锁

    return 0;
}

/**
 *修改密码 
 */
int change_password(PACK *pack, MYSQL mysql1) 
{
    MYSQL_RES          *result = NULL;
    PACK               *recv_pack = pack;
    MYSQL              mysql = mysql1;
    char               need[100];
    MYSQL_ROW          row;

    pthread_mutex_lock(&mutex);
    /**
     *  数据库操作 
     */
    sprintf(need, "select *from user_data where account = %d", recv_pack->data.send_account);  
    mysql_query(&mysql, need);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if (row) 
    {
        if (strcmp(recv_pack->data.read_buff, row[2]) == 0) 
        {
            recv_pack->data.recv_fd = atoi(row[4]);
            
            memset(need, 0, sizeof(need));
            sprintf(need, "update user_data set password = \"%s\" where account = %d", recv_pack->data.write_buff, recv_pack->data.send_account);
            mysql_query(&mysql, need);
            
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

    return -1;
}

/**
 *  找回密码 
 */

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
        strcpy(recv_pack->data.write_buff, "密码丢失，无法找回");
        
        /**
         *  you一个数据库操作
         */
        memset(need, 0, sizeof(need));
        sprintf(need, "select *from user_data where account = %d", recv_pack->data.send_account);
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
        sprintf(need, "select *from user_data where account = %d", recv_pack->data.send_account);
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
