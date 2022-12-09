#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 512

struct message{
	long msg_type;
	char msg_text[BUFFER_SIZE];
};

int main(){
	int qid;
	key_t key;
	struct message msg;
	
	/*
	 * 
	 * */
	if ((key = ftok(".", 'a')) == -1){
		perror("ftok");
		exit(1);
	}
	/*
	 *
	 * */
	if ((qid = msgget(key, IPC_CREAT|0666)) == -1){
		perror("msgget");
		exit(1);
	}
	printf("Open queue %d\n", qid);
}
