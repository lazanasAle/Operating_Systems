#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>
#include "scheduler_utils.cpp"



int main(int argc, char* argv[]){
    bip::shared_memory_object::remove("shm");

    bip::managed_shared_memory shm(bip::open_or_create, "shm", SHARED_SIZE);
    shared_data* shptr = shm.find_or_construct<shared_data>("sharedData")(shm.get_segment_manager());
    global_t=system_clock::now();
    fstream fin;
    if (argc == 2) {
        fin.open(argv[1]);
    }
    else{
        cerr<<"invalid usage\n";
        exit(1);
    }

    if(fin.is_open()){
        string tmp_name;
        string trash;
        size_t tmp_proc_num;
        while(getline(fin, tmp_name, ',')){
            fin>>tmp_proc_num;
            getline(fin, trash);
            process proc(tmp_name, 0, global_t, tmp_proc_num);
            shptr->proc_queue.push_back(proc);
        }
    }
    fin.close();
    vector<pid_t> processors;
    size_t num_pros=shptr->proc_queue.size();
    size_t size_q=(num_pros/num_processors)+1;

    for(size_t i=0; i<num_processors; ++i){
        pid_t pid=fork();
        if(pid==0){
            fcfs(shptr);
        }
        else if(pid>0){
            processors.push_back(pid);
        }
    }

    for (pid_t cpu : processors) {
        int status;
        waitpid(cpu, &status, 0);
    }
    bip::shared_memory_object::remove("shm");

    cout<<"WORKLOAD TIME: "<<duration_cast<seconds>(system_clock::now()-global_t).count()<<" secs\n";
    cout<<"scheduler exits\n";
}


