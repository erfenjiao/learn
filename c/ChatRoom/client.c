#include <stdio.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "my_pack.h"
#include "my_err.h"

PACK *send_pack;
PACK *recv_pack;
BOX *box;
FRIEND *list;
MESSAGE *message;
GROUP_MESSAGE *group_message;
GROUP *member_list;
GROUP_G *group_list;
FILE1 *file;
int sing;

void *thread_box           (void *sock_fd);  //消息盒子，上线要看看里面有没有消息
void *thread_list          (void *sock_fd);
void *thread_recv_fmes     (void *sock_fd);
void *thread_recv_gmes     (void *sock_fd);
void *thread_recv_file     (void *sock_fd);
void *thread_read_message  (void *sock_fd);
void *thread_member        (void *sock_fd);
void *thread_group_list    (void *sock_fd);



/* 用来发送数据的线程 */
void *sendMsg(void *sock_fd) 
{
    int               choose;
    char              password[20];
    char              ch;
    int               i = 0;
    struct termios    old, new;
    
    tcgetattr(0, &old);
    new = old;
    new.c_lflag &= ~(ECHO | ICANON);
    send_pack = (PACK *)malloc(sizeof(PACK));

    while (1) 
    {
        printf("                         +------------------------+\n");
        printf("                         |  1. Login              |\n");
        printf("                         |  2. Regisister         |\n");
        printf("                         |  3. Find Passwd        |\n");
        printf("                         |  4. Exit               |\n");
        printf("                         +------------------------+\n");
        printf("---------------------------Input your choose:-----------------------------\n");
        scanf("%d", &choose);
        switch (choose) 
        {
            case 1:
            {
                send_pack->type = L;
                printf("-----------------------------------------------------\n");
                printf("账号:\n");
                scanf("%d", &send_pack->data.send_account);
                getchar();
                printf("------------------------------------------------------\n");
                printf("密码:\n");
                i = 0;
                while (1) 
                {
                    tcsetattr(0, TCSANOW, &new);
                    scanf("%c", &ch);
                    tcsetattr(0, TCSANOW, &old);
                    if (ch == '\n') 
                    {
                        send_pack->data.read_buff[i] = '\0';
                        break;
                    }
                    send_pack->data.read_buff[i++] = ch;
                    printf("*");
                }
                printf("\n");

                memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0)           
                {
                    my_err("send", __LINE__);
                }

                pthread_mutex_lock(&mutex_cli);
                while (sing == 0) 
                {
                    pthread_cond_wait(&cond_cli, &mutex_cli);
                }
                pthread_mutex_unlock(&mutex_cli);

                printf("%s\n", send_pack->data.write_buff);    
                sing = 0;    
                break;
            }
           case 2:
            {
                send_pack->type = R;
                printf("-----------------------------------------------------\n");
                printf("昵称:\n");
                scanf("%s", send_pack->data.send_user);
                getchar();
                printf("-----------------------------------------------------\n");
                printf("密码:\n");
                i = 0;
                while (1) 
                {
                    tcsetattr(0, TCSANOW, &new);
                    scanf("%c", &ch);
                    tcsetattr(0, TCSANOW, &old);
                    if (ch == '\n') 
                    {
                        send_pack->data.read_buff[i] = '\0';
                        break;
                    }
                    send_pack->data.read_buff[i++] = ch;
                    printf("*");
                }
                printf("\n");
                    // scanf("%s", send_pack->data.read_buff);
                i = 0;
                printf("-----------------------------------------------------\n");
                printf("请再次输入密码:\n");
                while (1) 
                {
                    tcsetattr(0, TCSANOW, &new);
                    scanf("%c", &ch);
                    tcsetattr(0, TCSANOW, &old);
                    if (ch == '\n') 
                    {
                        password[i] = '\0';
                        break;
                    }
                    password[i++] = ch;
                    printf("*"); 
                }
                printf("\n");
                if (strcmp(password, send_pack->data.read_buff) == 0) 
                {
                    printf("两次输入 ，密码一致\n");               //将昵称和密码发送到服务器
                    if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                    {
                        my_err("send", __LINE__);
                    }

                    pthread_mutex_lock(&mutex_cli);
                    pthread_cond_wait(&cond_cli, &mutex_cli);
                    pthread_mutex_unlock(&mutex_cli);
                } 
                else 
                {
                    printf("两次密码不一致，请重新输入!\n") ;                    
                    continue;
                    getchar();
                }

                continue;
            }

           case 3:
            {
                send_pack->type = FIND_PASSWORD;
                printf("请输入你的账号:\n");
                scanf("%d", &send_pack->data.send_account);
                getchar();
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);
                pthread_mutex_unlock(&mutex_cli);

                printf("%s\n", send_pack->data.write_buff);
                
                getchar();
                continue;
            }
           case 4:
           {
                send_pack->type = EXIT;
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
                pthread_exit(0); //结束线程
            }
        }
        /**
         *  这个循环必不可少，因为如果放到上面，break仅仅只能跳出一个switch，但是在switch外面还套着
         *  一个while，这个循环必须跳出来，就要再来一个break；
         */
        if (choose > 4 || choose < 1)  
        {
            continue;   
        } 
        else if (choose == 1) 
        {
            if (strcmp(send_pack->data.write_buff, "password error") == 0) 
            {
                printf("密码错误或账号错误!!\n按下回车继续.....");
                getchar();
                continue;
            } 
            else 
            {
                printf("登陆成功!!\n");
                getchar();
                break;
            }
        } 
        else if (choose == 2) 
        {
            printf("-----注册成功!---------\n");
            printf("+---------------------+\n");
            printf("| 你的账号信息如下：     |\n");
            printf("+----------+----------+\n");
            printf("|user_name |   %s     |\n",send_pack->data.send_user);
            printf("| account  |   %d     |\n", send_pack->data.send_account);
            printf("| passwd   |   %s     |\n",password);
            printf("+---------------------+\n");
            getchar();
            continue;
        } 
    }
    while (1) 
    {
        printf("--------------------------------------------------------------------------------------------\n");
        printf("                                    欢迎，%s!",send_pack->data.send_user);
        printf("--------------------------------------------------------------------------------------------\n");
        printf("                         +-------------------------------+\n");
        printf("                         |  1.  Change_Passwd            |\n");
        printf("                         |  2.  Add_Firend               |\n");
        printf("                         |  3.  Delete_Firend            |\n");
        printf("                         |  4.  Friend_Plz               |\n");
        printf("                         |  5.  Look_List                |\n");
        printf("                         |  6.  User_chat                |\n");
        printf("                         |  7.  User_History             |\n");
        printf("                         |  8.  Create_Group             |\n");
        printf("                         |  9.  Add_Group                |\n");
        printf("                         |  10. Chat_Group               |\n");
        printf("                         |  11. Exit_Group               |\n");
        printf("                         |  12. Set_Admin                |\n");
        printf("                         |  13. Del_Admin                |\n");
        printf("                         |  14. Group_User               |\n");
        printf("                         |  15. Group_list               |\n");
        printf("                         |  16. ti_ren                   |\n");
        printf("                         |  17. Send_File                |\n");
        printf("                         |  18. Recv_File                |\n");
        printf("                         |  19. Logout                   |\n");
        printf("                         +-------------------------------+\n");
        printf("请选择:\n");
        printf("\n");
        scanf("%d", &choose);
        getchar();
        switch(choose) 
        {
            case 1:
            {
                send_pack->type = CHANGE_PASSWORD;
                printf("请输入原始密码:\n");
                i = 0;
                while (1) 
                {
                    tcsetattr(0, TCSANOW, &new);
                    scanf("%c", &ch);
                    tcsetattr(0, TCSANOW, &old);
                    if (ch == '\n') 
                    {
                        send_pack->data.read_buff[i] = '\0';
                        break;
                    }
                    send_pack->data.read_buff[i++] = ch;
                    printf("*");
                }
                printf("\n");
                printf("请输入修改后的密码:\n");
                i = 0;
                while (1) 
                {
                    tcsetattr(0, TCSANOW, &new);
                    scanf("%c", &ch);
                    tcsetattr(0, TCSANOW, &old);
                    if (ch == '\n') 
                    {
                        send_pack->data.write_buff[i] = '\0';
                        break;
                    }
                    send_pack->data.write_buff[i++] = ch;
                    printf("*");
                }
                printf("\n");
                int ret;
                if ((ret = send(*(int *)sock_fd, send_pack, sizeof(PACK), 0)) < 0) 
                {
                    my_err("send", __LINE__);
                }
                printf("^^%d\n", ret);
                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);
                pthread_mutex_unlock(&mutex_cli);
                printf("%s\n", recv_pack->data.write_buff);
                if (strcmp(send_pack->data.write_buff, "success") == 0) 
                {
                    printf("修改成功!\n");
                    printf("按下回车继续......\n");
                    getchar();
                } 
                else 
                {
                    printf("修改失败!!!\n");
                    printf("按下回车继续.......\n");
                    getchar();
                }
                memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
                break;
            }
            case 2:                   //添加好友
            {
                send_pack->type = ADD_FRIEND;
                printf("请输入你想添加的好友的账号:\n");             //send_account 自己的账号
                scanf("%d", &send_pack->data.recv_account);    //recv_account   friend_user
                getchar();

                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
                
                //printf("sendMsg:加锁前\n");
                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);
                pthread_mutex_unlock(&mutex_cli);
                
                //printf("sendMsg:解锁后\n");


                if (strcmp(send_pack->data.write_buff, "success") == 0) 
                {
                    printf("发送成功等待对方确认\n");
                    getchar();
                } 
                else 
                {
                    printf("对方已经是你的好友或者对方不存在\n");
                    getchar();
                }

                memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
                break;
            }

            case 3:             //删除好友
            {
                send_pack->type = DEL_FRIEND;
                printf("请输入想要删除好友的账号:\n");
                scanf("%d", &send_pack->data.recv_account);
                getchar();
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0)   //把要删除的账号从客户端发送到服务器，在deal()函数中处理
                {
                    my_err("send", __LINE__);
                }


                pthread_mutex_lock(&mutex_cli);                  
                pthread_cond_wait(&cond_cli, &mutex_cli);                   //等待服务器端处理完成，解锁
                pthread_mutex_unlock(&mutex_cli);


                if (strcmp(send_pack->data.write_buff, "success") == 0)     //下面的thread_read 函数把存储在服务器端的（recv_pack）传来的字符串拷贝到客户端这边的send_pack
                {
                    printf("删除成功!\n");
                    getchar();
                } 
                else 
                {
                    printf("你没有账号为%d的好友!!!\n", send_pack->data.recv_account);
                    getchar();
                }
                memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
                break;
            }
            case 4:                 //好友发来的请求
            {
                pthread_mutex_lock(&mutex_cli);
                send_pack->type = FRIENDS_PLZ;
                if (box->friend_number == 0) 
                {
                    printf("消息为空，没有好友发来请求\n");
                    pthread_mutex_unlock(&mutex);
                    getchar();
                } 
                else //消息盒子中有消息
                {
                    printf("开始处理消息\n");
                    for (int i = 0; i < box->friend_number; ++i) 
                    {
                        printf("%s\n", box->write_buff[i]);
                        send_pack->data.recv_account = box->plz_account[i];
                        printf("请选择:  1. 接受    2. 拒绝    3. 忽略\n");
                        scanf("%d", &choose);
                        getchar();
                        if (choose == 3) 
                        {
                            continue;
                        } 
                        else if (choose == 1) 
                        {
                            strcpy(send_pack->data.read_buff, "agree");
                            if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                            {
                                my_err("send", __LINE__);
                            }
                        } 
                        else if (choose == 2) 
                        {
                            strcpy(send_pack->data.read_buff, "disagree");
                            if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                            {
                                my_err("send", __LINE__);
                            }
                        }
                    }
                    box->friend_number = 0;
                    printf("处理完毕!!\n");
                    pthread_mutex_unlock(&mutex_cli);
                    memset(send_pack->data.write_buff ,  0  ,  sizeof(send_pack->data.write_buff));
                    memset(send_pack->data.write_buff ,  0  ,  sizeof(send_pack->data.read_buff));
                    getchar();
                }
                break;
            }
            case 5:         //查看好友列表
            {
                send_pack->type = LOOK_LIST;
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0)       //发送数据到服务器
                {
                    my_err("send", __LINE__);
                }

                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);                        //服务器处理好了的数据发送回来
                pthread_mutex_unlock(&mutex_cli);

                printf("%s\n", send_pack->data.write_buff);
                if (strcmp(send_pack->data.write_buff  ,  "success") == 0) 
                {
                    printf("好友列表:\n");
                    for (int i = 0 ; i < list->friend_number ; i++) 
                    {
                        printf("%d\t\t%s" , list->friend_account[i] , list->friend_nickname[i]);
                        if (list->friend_state[i] == 1) 
                        {
                             printf("在线\n");
                        } 
                        else 
                        {
                            printf("不在线\n");
                        }
                    }
                } 
                else
                {
                    printf("是空的！赶快丰富你的好友列表吧～\n");
                }
         \
                getchar();
                break;
            }
            /**
             *----------------------发送消息--------------------------- 
             */
            case 6:
            {
                send_pack->type = SEND_FMES;
                printf("请选择你要聊天的对象(输入账号):\n");
                scanf("%d", &send_pack->data.recv_account);
                getchar();
                printf("现在与好友%s (账号：%d)对话\n", send_pack->data.recv_user , send_pack->data.recv_account);
                while (1) 
                {
                    scanf("%s", send_pack->data.read_buff);
                    
                    if(strcmp(send_pack->data.read_buff , ":)") == 0)
                    {
                        printf("%s make a smiling face :)",send_pack->data.send_user);
                    }
                    
                    if (strcmp(send_pack->data.read_buff, "#bey") == 0) 
                    {
                        printf("与好友%s (账号：%d)的聊天结束\n",send_pack->data.recv_user , send_pack->data.recv_account);
                        break;
                    }
                    
                    if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                    {
                        my_err("send", __LINE__);
                    }
                    printf("私发消息,加锁前");

                    pthread_mutex_lock(&mutex_cli);
                    pthread_cond_wait(&cond_cli, &mutex_cli);
                    pthread_mutex_unlock(&mutex_cli);

                    if (strcmp(send_pack->data.write_buff, "fail") == 0) 
                    {
                        printf("没有账号为%d的好友\n", send_pack->data.recv_account);
                        break;
                    }
                }
                send_pack->data.recv_account = 0;
                getchar();
                break;
            }
            case 7:
            {
                if (box->talk_number == 0) 
                {
                    printf("你没有未看的好友消息!\n");
                } 
                else 
                {
                    for (int i = 0; i < box->talk_number; ++i) 
                    {
                        printf("账号%d:%s\n", box->send_account[i], box->read_buff[i]);
                    }
                    box->talk_number = 0;
                }
                getchar();
                break;
            }
/**
 *----------------------------create_group-----------------------------------
*/    
            case 8:     
            {
                send_pack->type = CREATE_GROUP;
                printf("请输入你要创建群的名称:\n");
                scanf("%s", send_pack->data.recv_user);
                getchar();
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
               
               
                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);
                pthread_mutex_unlock(&mutex_cli);
                
                
                if (strcmp(send_pack->data.write_buff, "success") == 0) 
                {
                    printf("成功创建群聊 \n");
                    printf("群号%d\t群名%s\n", send_pack->data.recv_account, send_pack->data.recv_user);
                } 
                else 
                {
                    printf("创建失败!!\n");
                }
                
                
                getchar();
                break;
            }

            case 9:
            {
                send_pack->type = ADD_GROUP;
                printf("请输入你想加的群的群号:\n");
                scanf("%d", &send_pack->data.recv_account);
                getchar();

                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                     my_err("send", __LINE__);
                }
                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);
                pthread_mutex_unlock(&mutex_cli);
                if (strcmp(send_pack->data.write_buff, "success") == 0) 
                {
                    printf("加入群%d成功!\n", send_pack->data.recv_account);
                } 
                else 
                {
                     printf("没有群号为%d的群!或你已加入该群!\n", send_pack->data.recv_account);
                }
               
                getchar();
                break;
            } 
/**
 * ---------------------------send_group_message ------------------------------
 */

            case 10:
            {
                send_pack->type = SEND_GMES;
                printf("请输入你要聊天的群号:\n");
                scanf("%d", &send_pack->data.recv_account);
                getchar();
                printf("开始发言:\n");

                while (1) 
                {
                    scanf("%s", send_pack->data.read_buff);
                    getchar();
                    if (strcmp(send_pack->data.read_buff, "#bey") == 0) 
                    {
                        printf("在群%d的发言结束\n", send_pack->data.recv_account);
                        break;
                    }

                    if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                    {
                        my_err("send", __LINE__);
                    }

                    pthread_mutex_lock(&mutex_cli);
                    pthread_cond_wait(&cond_cli, &mutex_cli);
                    pthread_mutex_unlock(&mutex_cli);

                    if (strcmp(send_pack->data.write_buff, "fail") == 0) 
                    {
                        printf("没有群号为%d的群\n", send_pack->data.recv_account);
                        break;
                    }
                }
                send_pack->data.recv_account = 0;

                getchar();
                break;
            }
        
            case 11:
            {
                send_pack->type = EXIT_GROUP;
                printf("请输入你要退出的群的群号:\n");
                
                scanf("%d", &send_pack->data.recv_account);
                getchar();
                
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
                
                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);
                pthread_mutex_unlock(&mutex_cli);
                
                if (strcmp(send_pack->data.write_buff, "success") == 0) 
                {
                    printf("已退出群%d\n", send_pack->data.recv_account);
                } 
                else 
                {
                    printf("你没有加入这个群\n");
                }
                
                getchar();
                break;
            }
            case 12:       
            {
                send_pack->type = S_A;
                printf("请输入群号:\n");
                scanf("%d", &send_pack->data.recv_account);
                getchar();
                
                printf("输入要被设置为管理员的群员账号:\n");
                scanf("%s", send_pack->data.read_buff);
                getchar();
                
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
                
                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);
                pthread_mutex_unlock(&mutex_cli);
                
                if (strcmp(send_pack->data.write_buff, "success") == 0) {
                    printf("账号为%s的成员被设置为群%d的管理员\n", send_pack->data.read_buff, send_pack->data.recv_account);
                } 
                else 
                {
                    printf("群%d里面没有%s这个成员\n", send_pack->data.recv_account, send_pack->data.read_buff);
                }
                
                
                getchar();
                break;
            }
            case 13:
            {
                send_pack->type = DEL_ADMIN;
                printf("请输入群号:\n");
                scanf("%d", &send_pack->data.recv_account);
                getchar();
                
                printf("输入要被取消管理员的群员账号:\n");
                scanf("%s", send_pack->data.read_buff);
                getchar();
                
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
                
                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);
                pthread_mutex_unlock(&mutex_cli);
                
                if (strcmp(send_pack->data.write_buff, "success") == 0) 
                {
                    printf("账号为%s的成员被取消为群%d的管理员\n", send_pack->data.read_buff, send_pack->data.recv_account);
                } 
                else 
                {
                    printf("群%d里面没有%s这个成员\n", send_pack->data.recv_account, send_pack->data.read_buff);
                }
                
                printf("按下回车继续.......");
                getchar();
                break;
            }
            case 14:
            {
                send_pack->type = LOOK_MEMBER;        //查看群成员
                printf("输入你想查看的群:\n");
                scanf("%d", &send_pack->data.recv_account);
                getchar();
                
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
                
                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);
                pthread_mutex_unlock(&mutex_cli);
                
                if (member_list->group_member_number == 0) 
                {
                    printf("没有群号为%d的群\n", recv_pack->data.recv_account);
                    printf("按下回车键继续.......");
                    getchar();
                    break;
                }
                for (int i = 0; i < member_list->group_member_number; ++i) 
                {
                    printf("%d\t%-20s\t", member_list->group_member_account[i], member_list->group_member_nickname[i]);
                    if (member_list->group_member_state[i] == 1) 
                    {
                        printf("群主\n");
                    } 
                    else if (member_list->group_member_state[i] == 2) 
                    {
                        printf("管理员\n");
                    } 
                    else 
                    {
                        printf("普通成员\n");
                    }
                }

                getchar();
                break;
            }

            case 15:
            {
                send_pack->type = LOOK_GROUP_LIST;
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) {
                    my_err("send", __LINE__);
                }

                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);
                pthread_mutex_unlock(&mutex_cli);

                if (group_list->number == 0) 
                {
                    printf("你还没加群\n");
                    printf("按下回车键继续........");
                    getchar();
                    break;
                }
                for (int i = 0; i < group_list->number; ++i) 
                {
                    printf("%d\t%-20s\t", group_list->group_account[i], group_list->group_name[i]);
                    if (group_list->group_state[i] == 1)
                    {
                        printf("群主\n");
                    } 
                    else if (group_list->group_state[i] == 2)
                    {
                        printf("管理员\n");
                    }
                     else 
                     {
                        printf("普通群员\n");
                     }
                }
                
                getchar();
                break;
            }
           

          
            case 16:
            {
                send_pack->type = DEL_MEMBER;
                printf("输入你的群号:\n");
                scanf("%d", &send_pack->data.recv_account);
                getchar();
                printf("输入你的要删除的成员账号:\n");
                scanf("%s", send_pack->data.read_buff);
                getchar();

                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }

                pthread_mutex_lock(&mutex_cli);
                pthread_cond_wait(&cond_cli, &mutex_cli);
                pthread_mutex_unlock(&mutex_cli);

                if (strcmp(send_pack->data.write_buff, "success") == 0) 
                {
                    printf("删除成功!!\n");
                } 
                else 
                {
                    printf("删除失败\n");
                }
                
                
                getchar();
                break;
            }
/**
 *-----------------------------------file----------------------------------- 
 */

            case 17 :
            {

                int fd;
                send_pack->type = SEND_FILE;
                printf("请输入你要发送文件的对象(账号)\n");
                scanf("%d",&send_pack->data.recv_account);
                getchar();
                printf("请输入你要发送的文件路径\n");
                scanf("%s",send_pack->data.write_buff);
                getchar();

                sing = 0;
                send_pack->data.cont = 0;
                fd = open(send_pack->data.write_buff , O_RDONLY);
                if(fd == -1)
                {
                    printf("文件打开失败！\n");
                    my_err("open",__LINE__);  
                }

                memset(send_pack->data.read_buff , 0 , sizeof(send_pack->data.read_buff));

                while(read(fd , send_pack->data.read_buff , 1023) > 0)
                {
                    if(send(*(int*)sock_fd , send_pack , sizeof(PACK) , 0) < 0)
                    {
                        my_err("send",__LINE__);
                    }
                    printf("sending now...%d\n",send_pack->data.cont);
                    memset(send_pack->data.read_buff , 0 , sizeof(send_pack->data.read_buff));

                    pthread_mutex_lock(&mutex);
                    while(sing == 0)
                    {
                        pthread_cond_wait(&cond_cli, &mutex_cli);
                    }
                    pthread_mutex_unlock(&mutex);

                    sing = 0;
                    send_pack->data.cont++;
                }

                close(fd);
                printf("close(fd)\n");

                //printf("$$$\n");
                send_pack->type = OK_FILE;
                sing = 0;
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
                printf("&&\n");
                // pthread_mutex_lock(&mutex_cli);
                // printf("sing = %d\n",sing);
                // while (sing == 0) 
                // {   
                //     pthread_cond_wait(&cond_cli, &mutex_cli);
                // }
                // pthread_mutex_unlock(&mutex_cli);
                sing = 0;
                printf("发送成功!!\n");
                break;
            }
            case 18:
            {
                send_pack->data.cont = 0;
                sing = 0;
                
                printf("账号%d的好友发来%s的文件\n", file->send_account, file->filename);
                printf("请选择:\n1. 接收 2. 拒绝\n");
                scanf("%d", &choose);
                getchar();
                if (choose == 1) 
                {
                    while (1) 
                    {        
                        send_pack->type = READ_FILE;

                        send_pack->data.recv_account = file->send_account;
                        memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
                        strcpy(send_pack->data.write_buff, file->filename);

                        printf("send_pack->data.cont = %d\n",send_pack->data.cont);

                        if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                        {
                            my_err("send", __LINE__);
                        }
                        printf("开始接收文件.....\n");


                        pthread_mutex_lock(&mutex_cli);
                        while (sing == 0) 
                        {
                            pthread_cond_wait(&cond_cli, &mutex_cli);
                        }
                        pthread_mutex_unlock(&mutex_cli);
                        send_pack->data.cont++;
                        sing = 0;
                        printf("send_pack->data.write_buff = %s\n",send_pack->data.write_buff);
                        if (strcmp(send_pack->data.write_buff, "ok") == 0) 
                        {
                            printf("文件接收完毕!!!\n");

                            break;
                        }
                    } 
                }
                else 
                {
                    printf("你已拒绝接收这个文件!!\n");
                    
                    getchar();
                }
            }
                break;
             case 19:
            {
                send_pack->type = EXIT;
                if (send(*(int *)sock_fd, send_pack, sizeof(PACK), 0) < 0) 
                {
                    my_err("send", __LINE__);
                }
                pthread_exit(0);
                break;
            }
                
            }
            
           
        }
    }


/**********************************************************************************************************************************
 **********************************************************************************************************************************
 **********************************************************************************************************************************
 **********************************************************************************************************************************
 **********************************************************************************************************************************
 **********************************************************************************************************************************
 ******************************************==============分界线==============*******************************************************
 **********************************************************************************************************************************
 **********************************************************************************************************************************
 **********************************************************************************************************************************
 **********************************************************************************************************************************
 **********************************************************************************************************************************
 **********************************************************************************************************************************
 */


void *readMsg(void *sock_fd) 
{
    pthread_t pid;
    int ret;
    group_list          =         (GROUP_G *)        malloc(sizeof(GROUP_G));
    member_list         =         (GROUP *)          malloc(sizeof(GROUP));
    list                =         (FRIEND *)         malloc(sizeof(FRIEND));
    box                 =         (BOX *)            malloc(sizeof(BOX));
    recv_pack           =         (PACK*)            malloc(sizeof(PACK));
    message             =         (MESSAGE *)        malloc(sizeof(MESSAGE));
    group_message       =         (GROUP_MESSAGE *)  malloc(sizeof(GROUP_MESSAGE));
    file                =         (FILE1 *)          malloc(sizeof(FILE1));
    file->have          =          0;
    while (1) 
    {
        memset(recv_pack, 0, sizeof(PACK));
        if ((ret = recv(*(int *)sock_fd, recv_pack, sizeof(PACK), MSG_WAITALL)) < 0)  //recv_pack:准备要发送过去的
        {
            my_err("recv", __LINE__);
        }

        if(recv_pack->type == L)
        {
            strcpy(send_pack->data.send_user, recv_pack->data.send_user);
                
            memset(send_pack->data.write_buff  ,  0  , sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff , recv_pack->data.write_buff);
            send_pack->data.send_fd  =  recv_pack->data.recv_fd;
            
            pthread_create(&pid, NULL, thread_box, sock_fd);
            pthread_join(pid, NULL);
            
            printf("亲爱的%s , 在您离线期间, 一共有有 %d 条消息 , %d 个好友请求 , %d 条群消息,快来查看吧～\n\n",recv_pack->data.send_user , box->talk_number, box->friend_number, box->number);
            
            pthread_mutex_lock(&mutex_cli);
            sing = 1;
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);

        }
        else if (recv_pack->type == ACCOUNT_ERROR)
        {
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);
            pthread_mutex_lock(&mutex_cli);
            sing = 1;
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
            break;  
        }
        else if (recv_pack->type == R)
        {
            send_pack->data.send_account = recv_pack->data.send_account;
            memset(send_pack->data.write_buff  ,  0  ,  sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff     ,     recv_pack->data.write_buff);

            printf("send_pack->data.write_buff = %s",send_pack->data.write_buff);

            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
     
        }
        else if (recv_pack->type == CHANGE_PASSWORD)
        {
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);

            printf("send_pack->data.write_buff = %s",send_pack->data.write_buff);
            
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if (recv_pack->type == FIND_PASSWORD)
        {
            memset(send_pack->data.write_buff , 0 , sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff , recv_pack->data.write_buff);      //recv_pack->data.write_buff：接收到的密码
            
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }
        
        else if (recv_pack->type == ADD_FRIEND)
        {
            memset(send_pack->data.write_buff  ,  0  ,  sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff     ,     recv_pack->data.write_buff);        
            //printf("client.c  thread_write:case:ADD_FRIENDS  加锁前\n");
            
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);

            //printf("client.c thread_write:case:ADD_FRIENDS  解锁后\n");
        }

        else if(recv_pack->type == DEL_FRIEND)
        {
            memset(send_pack->data.write_buff  ,  0  ,  sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff     ,     recv_pack->data.write_buff);       //"success"
            
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == SEND_FMES)
        {
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);

            printf("SEND_FMES:  %s",send_pack->data.write_buff);
            
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }
        
        else if(recv_pack->type == FRIENDS_PLZ)
        {
            pthread_mutex_lock(&mutex_cli);
            
            box->plz_account[box->friend_number] = recv_pack->data.send_account; 
            strcpy(box->write_buff[box->friend_number], recv_pack->data.read_buff);
            box->friend_number++;
            
            printf("您的好友%s发来一条消息,快来查看吧～\n",recv_pack->data.send_user);
            
            pthread_mutex_unlock(&mutex_cli);
        }
        
        else if(recv_pack->type == LOOK_LIST)
        {
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);
            
            memset(list, 0, sizeof(FRIEND));
            
            //printf("thread_list");
            
            pthread_create(&pid, NULL, thread_list, sock_fd);
            pthread_join(pid, NULL);
            
            //printf("LOOK_LIST 加锁前\n");
            
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == CREATE_GROUP)
        {
            strcpy(send_pack->data.recv_user, recv_pack->data.recv_user);
            
            send_pack->data.recv_account = recv_pack->data.recv_account;
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);
            
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == ADD_GROUP)
        {
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == EXIT_GROUP)
        {
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);
            
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }
        
        else if(recv_pack->type == DEL_MEMBER)
        {
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);
            
            
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == LOOK_GROUP_LIST)
        {
            pthread_create(&pid, NULL, thread_group_list, sock_fd);
            pthread_join(pid, NULL);
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == LOOK_MEMBER)
        {
            pthread_create(&pid, NULL, thread_member, sock_fd);
            pthread_join(pid, NULL);
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == S_A)
        {
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);
            
            
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == DEL_ADMIN)
        {
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == SEND_GMES)
        {
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);
            
            
            pthread_mutex_lock(&mutex_cli);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == RECV_GMES)
        {
            pthread_create(&pid, NULL, thread_recv_gmes, sock_fd);
            pthread_join(pid, NULL);  
        }

        else if(recv_pack->type == RECV_FMES)
        {
            pthread_create(&pid, NULL, thread_recv_fmes, sock_fd);  
            pthread_join(pid, NULL);     
        }
        else if(recv_pack->type == READ_MESSAGE)
        {
            pthread_mutex_lock(&mutex_cli);
            pthread_create(&pid, NULL, thread_read_message, sock_fd);
            pthread_join(pid, NULL);
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == SEND_FILE)
        {
        
            pthread_mutex_lock(&mutex_cli);
            sing = 1;
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
        }

        else if(recv_pack->type == RECV_FILE)
        {
            memset(file, 0, sizeof(file));
            file->send_account = recv_pack->data.send_account;
            
            strcpy(file->send_nickname, recv_pack->data.send_user);
            strcpy(file->filename     , recv_pack->data.write_buff);
            
            file->have = 1;
            printf("账号%d\t的好友给你发送了一个%s文件快去接收吧\n", file->send_account,file->filename);
            break;
        }

        else if(recv_pack->type == READ_FILE )
        {
            memset(send_pack->data.write_buff, 0, sizeof(send_pack->data.write_buff));
            strcpy(send_pack->data.write_buff, recv_pack->data.write_buff);

            //printf("READ_FILE:send_pack->data.write_buff = %s" , send_pack->data.write_buff);
            int fd = open("3", O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IXUSR);
            if(fd == -1)
            {
                my_err("open",__LINE__);
            }
            write(fd, recv_pack->data.read_buff, 1023);
            printf("write\n");
            // if(write(fd, recv_pack->data.write_buff, 1023) == -1)
            // {
            //     my_err("write" , __LINE__);
            // }
            close(fd);
            
            pthread_mutex_lock(&mutex_cli);
            sing = 1;
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
            
            break;
        }

        else if(recv_pack->type == OK_FILE)
        {
            pthread_mutex_lock(&mutex_cli);
            sing = 1;
            pthread_cond_signal(&cond_cli);
            pthread_mutex_unlock(&mutex_cli);
            break;
        }

        else if(recv_pack->type == EXIT)
        {
            printf("退出登录，拜拜～\n");
            pthread_exit(0);
        }
    }
}


void *thread_box(void *sock_fd) 
{
    if (recv(*(int *)sock_fd, box, sizeof(BOX), MSG_WAITALL) < 0) 
    {
        my_err("recv", __LINE__);
    }
    pthread_exit(0);
}



void *thread_list(void *sock_fd) 
{
    memset(list, 0, sizeof(FRIEND));
    
    if (recv(*(int *)sock_fd, list, sizeof(FRIEND), MSG_WAITALL) < 0) 
    {
        my_err("recv", __LINE__);
    }
    pthread_exit(0);
}



void *thread_recv_fmes(void *sock_fd) 
{
    if (recv_pack->data.send_account == send_pack->data.recv_account) 
    {
        printf("账号为%d昵称为%s的好友说:\t%s\n", recv_pack->data.send_account, recv_pack->data.send_user, recv_pack->data.read_buff);
    } 
    else 
    {
        box->send_account[box->talk_number] = recv_pack->data.send_account;
        strcpy(box->read_buff[box->talk_number++], recv_pack->data.read_buff);
        
        printf("消息盒子里来了一条好友消息!\n");
    }
    pthread_exit(0);
}



void *thread_recv_gmes(void *sock_fd) 
{
    if (recv_pack->data.recv_account == send_pack->data.recv_account) 
    {
        printf("群号%d 群名%s 账号%d 昵称%s:\t%s\n", recv_pack->data.recv_account, recv_pack->data.recv_user, recv_pack->data.send_account, recv_pack->data.send_user, recv_pack->data.read_buff);
    } 
    
    else 
    {
        printf("消息盒子里来了一条群消息\n");
        box->group_account[box->number] = recv_pack->data.recv_account;
        box->send_account1[box->number] = recv_pack->data.send_account;
        strcpy(box->message[box->number++], recv_pack->data.read_buff);
    }
}


void *thread_read_message(void *sock_fd) 
{
    if (recv(*(int *)sock_fd, message, sizeof(MESSAGE), MSG_WAITALL) < 0) 
    {
        my_err("recv", __LINE__);
    }
    pthread_exit(0);
}



void *thread_member(void *sock_fd) 
{
    memset(member_list, 0, sizeof(GROUP));
    if (recv(*(int *)sock_fd, member_list, sizeof(GROUP), MSG_WAITALL) < 0) 
    {
        my_err("recv", __LINE__);
    }
    pthread_exit(0);
}



void *thread_group_list(void *sock_fd) 
{
    memset(group_list, 0, sizeof(GROUP_G));
    if (recv(*(int *)sock_fd, group_list, sizeof(GROUP_G), MSG_WAITALL) < 0) 
    {
        my_err("recv", __LINE__);
    }
    pthread_exit(0);
}


void *thread_recv_file(void *sock_fd) {
    memset(file, 0, sizeof(file));
    file->send_account = recv_pack->data.send_account;
    strcpy(file->send_nickname, recv_pack->data.send_user);
    strcpy(file->filename, recv_pack->data.write_buff);
    file->have = 1;
    printf("账号%d\t昵称%s\t的好友给你发送了一个%s文件快去接收吧\n", file->send_account, file->send_nickname, file->filename);
    pthread_exit(0);
}

int main() 
{
    int                 sock_fd;
    pthread_t           pid1;
    pthread_t           pid2;
    struct sockaddr_in  seve;
    
    sing = 0;
    pthread_mutex_init(&mutex_cli, NULL);
    pthread_cond_init(&cond_cli, NULL);
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1)
    {
        my_err("socket",__LINE__);
    }
    // 2. 连接服务器
    seve.sin_family = AF_INET;
    seve.sin_port   = htons(10000);   // 大端端口
    inet_pton(AF_INET, "127.0.0.1", &seve.sin_addr.s_addr);
    int ret = connect(sock_fd, (struct sockaddr*)&seve, sizeof(seve));
    if(ret == -1)
    {
        my_err("connect",__LINE__);
    }
    pthread_create(&pid1, NULL, sendMsg, (void *)&sock_fd);
    pthread_create(&pid2, NULL, readMsg, (void *)&sock_fd);
    
    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);
    
	return 0;
}

