#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>

typedef struct{
    void** queue;
    size_t front;
    size_t rear;
    size_t length;
    size_t it_c;
}circ_queue;



void* shmalloc(size_t length){
    int protection = PROT_READ|PROT_WRITE;
    int visibility = MAP_SHARED|MAP_ANONYMOUS;
    return mmap(NULL, length, protection, visibility, -1, 0);
}



void make_queue(circ_queue* cq, size_t length, size_t obj_length, void* (*allocator)(size_t)){
    cq->queue=(void**)allocator(length*sizeof(void*));
    for(size_t j=0; j<length; ++j){
        cq->queue[j]=allocator(obj_length);
    }
    cq->front=0;
    cq->rear=0;
    cq->it_c=0;
    cq->length=length;
}

bool full(circ_queue* cq){
    return (cq->it_c>=cq->length);
}


bool empty(circ_queue* cq){
    return(cq->it_c<=0);
}

void enqueue(circ_queue* cq, void* item, size_t obj_length){
    if(!full(cq)){
        memcpy(cq->queue[cq->rear], item, obj_length);
        if(cq->rear<cq->length-1)
            cq->rear++;
        else
            cq->rear=0;
        cq->it_c++;
    }
}


void* dequeue(circ_queue* cq, size_t obj_length){
    if(!empty(cq)){
        void* value=malloc(obj_length);
        memcpy(value, cq->queue[cq->front], obj_length);
        if(cq->front<cq->length-1)
            cq->front++;
        else
            cq->front=0;
        cq->it_c--;
        return value;
    }
    return NULL;
}


void destroy_queue(circ_queue* cq, size_t obj_length, int (*destroyer)(void*, size_t)){
    for(size_t j=0; j<cq->length; ++j){
        destroyer(cq->queue[j], obj_length);
    }
}


int local_destroyer(void* ptr, size_t mem_size){
    free(ptr);
    return 0;
}
