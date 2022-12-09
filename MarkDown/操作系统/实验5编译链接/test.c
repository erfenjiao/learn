#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "calc.h"

int main()
{
    pthread_t id1;
    pthread_t id2;
    int i;
    int ret;

    sem_mutex_init();
    
    /*create the producer thread*/
    ret = pthread_create(&id1, NULL, producer, NULL);
    if(ret != 0)
    {
        printf("producer creation failed \n");
        exit(1);
    }
    
    /*create the consumer thread*/
    ret = pthread_create(&id2, NULL, consumer, NULL);
    if(ret != 0)
    {
        printf("consumer creation failed \n");
        exit(1);
    }

    pthread_join(id1,NULL);
    pthread_join(id2,NULL);

    exit(0);
}
