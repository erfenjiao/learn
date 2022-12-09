#ifndef CALC_H
#define calc_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define M 32      

#define P(x)     sem_wait(&x)
#define V(x)    sem_post(&x)

void print();
void *producer();
void *consumer();
void sem_mutex_init();

#endif
