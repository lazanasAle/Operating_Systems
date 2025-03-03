#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <ctime>
#include <chrono>
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


time_point<system_clock> global_t;
const size_t num_processors=4;


enum proc_status{
    PROC_NEW,
    PROC_STOPPED,
    PROC_RUNNING,
    PROC_EXITED
};




struct process{
    string name;
    size_t pid;
    proc_status status;
    size_t demands;
    time_point<system_clock> t_submission, t_start, t_end;
    process(){}
    process(string name_, size_t pid_, time_point<system_clock> t_submission_, size_t demands_): name(name_), pid(pid_), t_submission(t_submission_), status(PROC_NEW), demands(demands_){}
};

struct shared_data{
    using shared_allocator_t = bip::allocator<process, bip::managed_shared_memory::segment_manager>;
    using shared_list_t = bc::list<process, shared_allocator_t>;
    shared_list_t proc_queue;
    bip::interprocess_mutex mtx;
    size_t available_processors;
    shared_data(const shared_allocator_t& alloc) : proc_queue(alloc), available_processors(num_processors) {}

};


void fcfs(shared_data*& shptr){
    process proc;
    int status;
    while(true){
        shptr->mtx.lock();
        if(shptr->proc_queue.empty()){
            shptr->mtx.unlock();
            break;
        }
        proc = shptr->proc_queue.front();
        shptr->proc_queue.pop_front();
        if(proc.demands<=shptr->available_processors){
            if(proc.status==PROC_NEW){
                shptr->available_processors-=proc.demands;
                shptr->mtx.unlock();
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
                    shptr->mtx.lock();
                    shptr->available_processors+=proc.demands;
                    shptr->mtx.unlock();
                    cout<<"PID "<<pid<< "CMD: "<<proc.name<<"\n";
                    cout<<"\tElapsed time = "<<duration_cast<seconds>(proc.t_end-proc.t_submission).count()<< "secs\n";
                    cout<<"\tExecution time = "<<duration_cast<seconds>(proc.t_end-proc.t_start).count()<<"secs\n";
                    cout<<"\tWorkload time = "<<duration_cast<seconds>(proc.t_end-global_t).count()<<"secs\n";
                }
            }
        }
        else{
            shptr->mtx.unlock();
            if(proc.demands<=num_processors){
                shptr->mtx.lock();
                shptr->proc_queue.push_back(proc);
                shptr->mtx.unlock();
            }
        }
    }
    exit(0);
}




