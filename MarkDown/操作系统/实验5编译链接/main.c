#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "calc.h"

#define M 32      

#define P(x)     sem_wait(&x)
#define V(x)    sem_post(&x)

int in = 0;    
int out = 0;   

int buff[M] = {0};  

sem_t empty_sem;
sem_t full_sem; 
pthread_mutex_t mutex;

/*
 *output the buffer
 */
void print()
{
    int i;
    for(i = 0; i < M; i++)
        printf("%d ", buff[i]);
    printf("\n");
}

/*
 *producer
 */
void *producer()
{
    for(;;)
    {
        sleep(1);

        P(empty_sem);
        pthread_mutex_lock(&mutex);

        in = in % M;
        printf("(+)produce a product. buffer:");

        buff[in] = 1;
        print();
        ++in;

        pthread_mutex_unlock(&mutex);
        V(full_sem);
    }
}

/*
 *consumer
 */
void *consumer()
{
    for(;;)
    {
        sleep(1);

        P(full_sem);
        pthread_mutex_lock(&mutex);

        out = out % M;
        printf("(-)consume a product. buffer:");

        buff[out] = 0;
        print();
        ++out;

        pthread_mutex_unlock(&mutex);
        V(empty_sem);
    }
}

void sem_mutex_init()
{
    /*
     *semaphore initialize
     */
    int init1 = sem_init(&empty_sem, 0, M);
    int init2 = sem_init(&full_sem, 0, 0);
    if( (init1 != 0) && (init2 != 0))
    {
        printf("sem init failed \n");
        exit(1);
    }
    /*
     *mutex initialize
     */
    int init3 = pthread_mutex_init(&mutex, NULL);
    if(init3 != 0)
    {
        printf("mutex init failed \n");
        exit(1);
    }
    
}
