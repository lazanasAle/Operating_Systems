#include <stdlib.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "circ_queue_generic.c"

typedef struct sigaction sig_action;
typedef pthread_mutex_t mutex_t;

typedef enum {
    PROC_NEW,
    PROC_STOPPED,
    PROC_RUNNING,
    PROC_EXITED
}proc_status;

typedef enum {
    RR,
    FCFS,
    RRAFF
}policy;

typedef struct{
    char name[20];
    pid_t pid;
    proc_status status;
    time_t t_submission, t_start, t_end;
}process;


time_t global_t;
size_t time_slice=4;
const size_t num_processors=4;
size_t size_q;
mutex_t* shared_mtx;
circ_queue* prlist;


volatile sig_atomic_t chld_completed=0;
policy pol;
process* running;


void termination_achieved(){
    running->status=PROC_EXITED;
    running->t_end=time(0);
    printf("PID: %d CMD: %s\n", running->pid, running->name);
    printf("\tElapsed time = %ld secs\n", running->t_end-running->t_submission);
    printf("\tExecution time = %ld secs\n", running->t_end-running->t_start);
    printf("\tWorkload time = %ld secs\n", running->t_end-global_t);
}

void signal_handler(int signo){
    if(pol!=FCFS){
        chld_completed=1;
    }
}



void rr(){
    sig_action sa;
    sa.sa_handler=signal_handler;
    sa.sa_flags=SA_NOCLDSTOP;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Failed to set up signal handler.\n");
        return;
    }

    while(true){
        pthread_mutex_lock(shared_mtx);
        if(empty(prlist)){
            pthread_mutex_unlock(shared_mtx);
            break;
        }
        process* proc=(process*)dequeue(prlist, sizeof(process));
        pthread_mutex_unlock(shared_mtx);
        if(proc->status==PROC_NEW && proc->pid==-1){
            proc->t_start=time(0);
            chld_completed=0;
            pid_t pid=fork();
            if(pid==0){
                printf("Processor: %d executing %s\n", getppid(), proc->name);
                execl(proc->name, proc->name, NULL);
            }
            else if(pid>0){
                proc->pid=pid;
                proc->status=PROC_RUNNING;
                running=proc;
                struct timespec ts = {(time_t)time_slice, 0};
                nanosleep(&ts, NULL);
                if(chld_completed==0){
                    int try_kill=kill(pid, SIGSTOP);
                    if(try_kill==0){
                        proc->status=PROC_STOPPED;
                        printf("Processor: %d stopped: %s\n", getpid(), proc->name);
                        pthread_mutex_lock(shared_mtx);
                        enqueue(prlist, proc, sizeof(process));
                        pthread_mutex_unlock(shared_mtx);
                    }
                    else if(try_kill==-1 && errno==ESRCH){
                        termination_achieved();
                    }
                }
                else{
                    termination_achieved();
                }
            }
        }
        else{
            chld_completed=0;
            kill(proc->pid, SIGCONT);
            printf("Processor: %d resumed: %s\n", getpid(), proc->name);
            proc->status=PROC_RUNNING;
            running=proc;
            struct timespec ts = {(time_t)time_slice, 0};
            nanosleep(&ts, NULL);
            if(chld_completed==0){
                int try_kill=kill(proc->pid, SIGSTOP);
                if(try_kill==0){
                    proc->status=PROC_STOPPED;
                    printf("Processor: %d stopped: %s\n", getpid(), proc->name);
                    pthread_mutex_lock(shared_mtx);
                    enqueue(prlist, proc, sizeof(process));
                    pthread_mutex_unlock(shared_mtx);
                }
                else if(try_kill==-1 && errno==ESRCH){
                    termination_achieved();
                }
            }
            else{
                termination_achieved();
            }
        }
    }
}




void fcfs(){
    process* proc;
    int status;
    sig_action sa;
    sa.sa_handler=signal_handler;
    sa.sa_flags=SA_NOCLDSTOP;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Failed to set up signal handler.\n");
        return;
    }

    while(true){
        pthread_mutex_lock(shared_mtx);
        if(empty(prlist)){
            pthread_mutex_unlock(shared_mtx);
            break;
        }
        proc=(process*)dequeue(prlist, sizeof(process));
        pthread_mutex_unlock(shared_mtx);
        if(proc->status==PROC_NEW){
            proc->t_start = time(0);
            pid_t pid = fork();
            if(pid<0){
                perror("fork_failed\n");
                exit(-1);
            }
            else if(pid==0){
                printf("Processor: %d executing %s\n", getppid(), proc->name);
                execl(proc->name, proc->name, NULL);
            }
            else{
                proc->pid=pid;
                proc->status=PROC_RUNNING;
                waitpid(proc->pid, &status, 0);
                proc->status=PROC_EXITED;
                proc->t_end=time(0);
                printf("PID  %d CMD: %s\n", pid, proc->name);
                printf("\tElapsed time = %ld secs\n", proc->t_end-proc->t_submission);
                printf("\tExecution time = %ld secs\n", proc->t_end-proc->t_start);
                printf("\tWorkload time = %ld secs\n", proc->t_end-global_t);
            }
        }
    }
}


void rraff(){
    circ_queue* local_list=(circ_queue*)malloc(sizeof(circ_queue));
    make_queue(local_list, size_q, sizeof(process), malloc);
    sig_action sa;
    sa.sa_handler=signal_handler;
    sa.sa_flags=SA_NOCLDSTOP;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Failed to set up signal handler.\n");
        return;
    }
    for(size_t j=0; j<size_q; ++j){
        pthread_mutex_lock(shared_mtx);
        if(empty(prlist)){
            pthread_mutex_unlock(shared_mtx);
            break;
        }
        else{
            process* proc=(process*)dequeue(prlist, sizeof(process));
            pthread_mutex_unlock(shared_mtx);
            enqueue(local_list, proc, sizeof(process));
        }
    }

    while(!empty(local_list)){
        process* proc=(process*)dequeue(local_list, sizeof(process));
        if(proc->status==PROC_NEW && proc->pid==-1){
            proc->t_start=time(0);
            chld_completed=0;
            pid_t pid=fork();
            if(pid==0){
                printf("Processor: %d executing %s\n", getppid(), proc->name);
                execl(proc->name, proc->name, NULL);
            }
            else if(pid>0){
                proc->pid=pid;
                proc->status=PROC_RUNNING;
                running=proc;
                struct timespec ts = {(time_t)time_slice, 0};
                nanosleep(&ts, NULL);
                if(chld_completed==0){
                    int try_kill=kill(pid, SIGSTOP);
                    if(try_kill==0){
                        proc->status=PROC_STOPPED;
                        printf("Processor: %d stopped: %s\n", getpid(), proc->name);
                        enqueue(local_list, proc, sizeof(process));
                    }
                    else if(try_kill==-1 && errno==ESRCH){
                        termination_achieved();
                    }
                }
                else{
                    termination_achieved();
                }
            }
        }
        else{
            chld_completed=0;
            kill(proc->pid, SIGCONT);
            printf("Processor: %d resumed: %s\n", getpid(), proc->name);
            proc->status=PROC_RUNNING;
            running=proc;
            struct timespec ts = {(time_t)time_slice, 0};
            nanosleep(&ts, NULL);
            if(chld_completed==0){
                int try_kill=kill(proc->pid, SIGSTOP);
                if(try_kill==0){
                    proc->status=PROC_STOPPED;
                    printf("Processor: %d stopped: %s\n", getpid(), proc->name);
                    enqueue(local_list, proc, sizeof(process));
                }
                else if(try_kill==-1 && errno==ESRCH){
                    termination_achieved();
                }
            }
            else{
                termination_achieved();
            }
        }
    }
    free(local_list);
}

