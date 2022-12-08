
#ifndef _MY_FRIENDS_H
#define _MY_FRIENDS_H
#include"/usr/local/mysql/include/mysql.h"
#include "my_pack.h"

int add_fir(PACK *pack, MYSQL mysql1);

int del_friend(PACK *pack, MYSQL mysql1);

int friends_plz(PACK *pack, MYSQL mysql1);

FRIEND *look_list(PACK *pack, MYSQL mysql1);

#endif
