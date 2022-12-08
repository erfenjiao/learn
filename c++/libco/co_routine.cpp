#include"co_routine.h"




#include<stdlib.h>



extern "C"
{
    extern void coctx_swap(coctx_t * , coctx_t *) asm("coctx_swap");
};

using namespace std;

int co_create(stCoRoutinue_t ** co , const stCoRoutineAttr_t * attr , void *(*routine)(void*) , void *arg)
{

}


