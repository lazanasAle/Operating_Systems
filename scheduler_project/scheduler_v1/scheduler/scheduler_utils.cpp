#include <iostream>
#include <string>
#include <semaphore>
#include <unistd.h>
#include <sys/wait.h>
#include <ctime>
#include <chrono>
#include <csignal>
#include <errno.h>
#include <list>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <sys/wait.h>
#define SHARED_SIZE 65536
using namespace std;
using namespace std::chrono;
namespace bip = boost::interprocess;
namespace bc = boost::container;

typedef struct sigaction sig_action;

time_point<system_clock> global_t;
size_t time_slice=4;
counting_semaphore<10> proc_wake(0);
const size_t num_processors=4;
size_t size_q;



enum proc_status{
    PROC_NEW,
    PROC_STOPPED,
    PROC_RUNNING,
    PROC_EXITED
};


enum policy{
    RR,
    FCFS,
    RRAFF
};


struct process{
    string name;
    size_t pid;
    proc_status status;
    time_point<system_clock> t_submission, t_start, t_end;
    process(){}
    process(string name_, size_t pid_, time_point<system_clock> t_submission_): name(name_), pid(pid_), t_submission(t_submission_), status(PROC_NEW){}
};

struct shared_data{
    using shared_allocator_t = bip::allocator<process, bip::managed_shared_memory::segment_manager>;
    using shared_list_t = bc::list<process, shared_allocator_t>;
    shared_list_t proc_queue;
    bip::interprocess_mutex mtx;

    shared_data(const shared_allocator_t& alloc) : proc_queue(alloc) {}

};


volatile sig_atomic_t proc_finished=0;
policy pol;
process* running;

void termination_achieved(){
    running->status=PROC_EXITED;
    running->t_end=system_clock::now();
    cout<<"PID "<<running->pid<< " CMD: "<<running->name<<"\n";
    cout<<"\tElapsed time = "<<duration_cast<seconds>(running->t_end-running->t_submission).count()<< "secs\n";
    cout<<"\tExecution time = "<<duration_cast<seconds>(running->t_end-running->t_start).count()<<"secs\n";
    cout<<"\tWorkload time = "<<duration_cast<seconds>(running->t_end-global_t).count()<<"secs\n";
}

void signal_handler(int signo, siginfo_t* info, void* context){
    if(pol!=FCFS && info->si_code==CLD_EXITED){
        proc_finished=1;
        proc_wake.release();
    }
}


//η λιστα ξεχωρα καθε process

list<process> local_q;

void rr(shared_data*& shptr){
    sig_action sa;
    sa.sa_handler=nullptr;
    sa.sa_sigaction=signal_handler;
    sa.sa_flags=SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        cerr << "Failed to set up signal handler.\n";
        return;
    }

    while(true){
        shptr->mtx.lock();
        if(shptr->proc_queue.empty()){
            shptr->mtx.unlock();
            break;
        }
        auto proc=shptr->proc_queue.front();
        shptr->proc_queue.pop_front();
        shptr->mtx.unlock();
        if(proc.status==PROC_NEW){
            proc.t_start=system_clock::now();
            proc_finished=0;
            pid_t pid=fork();
            if(pid==0){
                cout<<"Processor: "<<getppid()<<" executing "<<proc.name<<"\n";
                execl(proc.name.c_str(), proc.name.c_str(), nullptr);
            }
            else{
                proc.pid=pid;
                proc.status=PROC_RUNNING;
                running=&proc;
                proc_wake.try_acquire_for(seconds(time_slice));
                if(proc_finished==0){
                    int try_kill=kill(pid, SIGSTOP);
                    if(try_kill==0){
                        proc.status=PROC_STOPPED;
                        cout<<"Processor: "<<getpid()<<" stopped: "<<proc.name<<"\n";
                        shptr->mtx.lock();
                        shptr->proc_queue.push_back(proc);
                        shptr->mtx.unlock();
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
        else if(proc.status==PROC_STOPPED){
            proc_finished=0;
            int try_kill=kill(proc.pid, SIGCONT);
            if(try_kill==0){
                cout<<"Processor: "<<getpid()<<" resumed: "<<proc.name<<"\n";
                proc.status=PROC_RUNNING;
                running=&proc;
                proc_wake.try_acquire_for(seconds(time_slice));
                if(proc_finished==0){
                    int try_kill=kill(proc.pid, SIGSTOP);
                    if(try_kill==0){
                        proc.status=PROC_STOPPED;
                        cout<<"Processor: "<<getpid()<<" stopped: "<<proc.name<<"\n";
                        shptr->mtx.lock();
                        shptr->proc_queue.push_back(proc);
                        shptr->mtx.unlock();
                    }
                    else if(try_kill==-1 && errno==ESRCH){
                        termination_achieved();
                    }
                }
                else{
                    termination_achieved();
                }
            }
            else if(try_kill==-1 && errno==ESRCH){
                termination_achieved();
            }
        }
    }
}

void fcfs(shared_data*& shptr){
    process proc;
    int status;
    sig_action sa;
    sa.sa_handler=nullptr;
    sa.sa_sigaction=signal_handler;
    sa.sa_flags=SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        cerr << "Failed to set up signal handler.\n";
        return;
    }

    while(true){
        shptr->mtx.lock();
        if(shptr->proc_queue.empty()){
            shptr->mtx.unlock();
            break;
        }
        proc = shptr->proc_queue.front();
        shptr->proc_queue.pop_front();
        shptr->mtx.unlock();
        if(proc.status==PROC_NEW){
            proc.t_start = system_clock::now();
            pid_t pid = fork();
            if(pid<0){
                cerr<<"fork_failed\n";
                exit(-1);
            }
            else if(pid==0){
                cout<<"Processor: "<<getppid()<<" executing "<<proc.name<<"\n";
                execl(proc.name.c_str(), proc.name.c_str(), nullptr);
            }
            else{
                proc.pid=pid;
                proc.status=PROC_RUNNING;
                waitpid(proc.pid, &status, 0);
                proc.status=PROC_EXITED;
                proc.t_end=system_clock::now();
                cout<<"PID "<<pid<< " CMD: "<<proc.name<<"\n";
                cout<<"\tElapsed time = "<<duration_cast<seconds>(proc.t_end-proc.t_submission).count()<< "secs\n";
                cout<<"\tExecution time = "<<duration_cast<seconds>(proc.t_end-proc.t_start).count()<<"secs\n";
                cout<<"\tWorkload time = "<<duration_cast<seconds>(proc.t_end-global_t).count()<<"secs\n";
            }
        }
    }
}



void rraff(shared_data*& shptr){
    sig_action sa;
    sa.sa_handler=nullptr;
    sa.sa_sigaction=signal_handler;
    sa.sa_flags=SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        cerr << "Failed to set up signal handler.\n";
        return;
    }

    process proc;
    for(size_t i=0; i<size_q; i++){
        shptr->mtx.lock();
        if(shptr->proc_queue.empty() && local_q.empty()){
            shptr->mtx.unlock();
            break;
        }
        if(!(shptr->proc_queue.empty())){
            proc = shptr->proc_queue.front();
            shptr->proc_queue.pop_front();
            shptr->mtx.unlock();
            local_q.push_back(proc);
        }
        else{
            shptr->mtx.unlock();
            break;
        }
    }

    while(true){
        if(local_q.empty()){
            break;
        }
        auto tmp=local_q.front();
        local_q.pop_front();

        if(tmp.status==PROC_NEW){
            tmp.t_start=system_clock::now();
            proc_finished=0;
            pid_t pid=fork();
            if(pid==0){
                cout<<"Processor: "<<getppid()<<" executing "<<tmp.name<<"\n";
                execl(tmp.name.c_str(), tmp.name.c_str(), nullptr);
            }
            else{
                tmp.pid=pid;
                tmp.status=PROC_RUNNING;
                running=&tmp;
                proc_wake.try_acquire_for(seconds(time_slice));
                if(proc_finished==0){
                    int try_kill=kill(pid, SIGSTOP);
                    if(try_kill==0){
                        tmp.status=PROC_STOPPED;
                        cout<<"Processor: "<<getpid()<<" stopped: "<<tmp.name<<"\n";
                        local_q.push_back(tmp);
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
            proc_finished=0;
            int try_kill=kill(tmp.pid, SIGCONT);
            if(try_kill==0){
                cout<<"Processor: "<<getpid()<<" resumed: "<<tmp.name<<"\n";
                tmp.status=PROC_RUNNING;
                running=&tmp;
                proc_wake.try_acquire_for(seconds(time_slice));
                if(proc_finished==0){
                    int try_kill=kill(tmp.pid, SIGSTOP);
                    if(try_kill==0){
                        tmp.status=PROC_STOPPED;
                        cout<<"Processor: "<<getpid()<<" stopped: "<<tmp.name<<"\n";
                        local_q.push_back(tmp);
                    }
                    else if(try_kill==-1 && errno==ESRCH){
                        termination_achieved();
                    }
                }
                else{
                    termination_achieved();
                }
            }
            else if(try_kill==-1 && errno==ESRCH){
                termination_achieved();
            }
        }
    }
}
