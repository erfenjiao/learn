#ifndef _CO_ROUTINE_H_
#define _CO_ROUTINE_H

#include<stdint.h>
#include<pthread.h>


struct stCoRoutinue_t;
struct stShareStack_t;

struct stCoRoutineAttr_t
{
    int stack_size;
    stShareStack_t* share_stack;
    stCoRoutineAttr_t()
    {
        stack_size = 128 * 1024;
        share_stack = NULL;
    }
}__attritube__((packed));


//co_routine

int co_create(stCoRoutinue_t ** co , const stCoRoutineAttr_t * attr , void *(*routine)(void*) , void *arg);

void co_resume(stCoRoutinue_t * co);

void yeild(stCoRoutinue_t * co);

void co_release(stCoRoutinue_t * co);


#endif