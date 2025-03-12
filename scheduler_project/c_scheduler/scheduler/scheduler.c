#include <stdlib.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "scheduler_utils.c"

void choose_policy();

int main(int argc, char* argv[]){
    shared_mtx = (mutex_t*) shmalloc(sizeof(mutex_t));
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(shared_mtx, &attr);
    prlist = (circ_queue*) shmalloc(sizeof(circ_queue));
    make_queue(prlist, 20, sizeof(process), shmalloc);
    pol=RR;
    global_t=time(0);
    FILE* fin;
    if(argc==1){
        fprintf(stderr, "invalid usage\n");
        exit(1);
    }
    else if(argc==2){
        fin=fopen(argv[1], "r");
        if(fin==NULL){
            fprintf(stderr, "invalid input filename\n");
            exit(1);
        }
    }

    else if (argc > 2) {
        if (!strcmp(argv[1],"FCFS")) {
            pol = FCFS;
            fin=fopen(argv[2], "r");
            if(fin==NULL){
                fprintf(stderr, "invalid input filename\n");
                exit(1);
            }
        }
        else if (!strcmp(argv[1],"RR")) {
            pol = RR;
            time_slice = atoi(argv[2])/1000;
            fin=fopen(argv[3], "r");
            if(fin==NULL){
                fprintf(stderr, "invalid input filename\n");
                exit(1);
            }
        }
        else if(!strcmp(argv[1], "RRAFF")){
            pol = RRAFF;
            time_slice = atoi(argv[2])/1000;
            fin=fopen(argv[3], "r");
            if(fin==NULL){
                fprintf(stderr, "invalid input filename\n");
                exit(1);
            }
        }
        else {
            fprintf(stderr, "invalid usage\n");
            exit(1);
        }

        if(time_slice<1){
            fprintf(stderr, "too small time_slice\n");
            exit(1);
        }
    }

    if(fin!=NULL){
        char tmp_name[20];
        while(fscanf(fin, "%s", tmp_name)!=EOF){
            process* proc=(process*)malloc(sizeof(process));
            strcpy(proc->name, tmp_name);
            proc->pid=-1;
            proc->status=PROC_NEW;
            proc->t_submission=time(0);
            enqueue(prlist, proc, sizeof(process));
            free(proc);
        }
    }
    fclose(fin);
    pid_t processors[num_processors];
    size_t num_pros=prlist->it_c;
    size_q=(num_pros/num_processors)+1;
    for(size_t i=0; i<num_processors; ++i){
        processors[i]=fork();
        if(processors[i]==0){
            choose_policy();
        }
    }
    for(size_t j=0; j<num_processors; ++j){
        int status;
        waitpid(processors[j], &status, 0);
    }
    pthread_mutex_destroy(shared_mtx);
    destroy_queue(prlist, sizeof(process), munmap);
    munmap(prlist, sizeof(circ_queue));
    munmap(shared_mtx, sizeof(mutex_t));
    printf("WORKLOAD TIME: %ld secs\n", time(0)-global_t);
    printf("scheduler exits\n");

}


void choose_policy(){
    switch(pol){
    case FCFS:
        fcfs();
        break;
    case RR:
        rr();
        break;
    case RRAFF:
        rraff();
        break;
    }
    exit(0);
}
