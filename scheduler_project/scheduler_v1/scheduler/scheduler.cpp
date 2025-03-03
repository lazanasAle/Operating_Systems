#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>
#include <map>
#include "scheduler_utils.cpp"

void choose_policy(shared_data*& shptr);


int main(int argc, char* argv[]){
    map<string, policy> pol_map;
    pol_map["FCFS"]=FCFS;
    pol_map["RR"]=RR;
    pol_map["RRAFF"]=RRAFF;

    bip::shared_memory_object::remove("shm");

    bip::managed_shared_memory shm(bip::open_or_create, "shm", SHARED_SIZE);
    shared_data* shptr = shm.find_or_construct<shared_data>("sharedData")(shm.get_segment_manager());
    pol=RR;
    global_t=system_clock::now();
    fstream fin;
    if (argc == 1) {
        cerr<<"invalid usage\n";
        exit(1);
    }
    else if (argc == 2) {
        fin.open(argv[1]);
        if(!fin.is_open()){
            cerr<<"invalid input filename\n";
            exit(1);
        }
    }
    else if (argc > 2) {
        if (!strcmp(argv[1],"FCFS")) {
            pol = FCFS;
            fin.open(argv[2]);
            if(!fin.is_open()){
                cerr<<"invalid input filename\n";
                exit(1);
            }
        }
        else if (!strcmp(argv[1],"RR") || !strcmp(argv[1],"RRAFF")) {
            pol = pol_map[argv[1]];
            time_slice = stoi(argv[2])/1000;
            fin.open(argv[3]);
            if(!fin.is_open()){
                cerr<<"invalid input filename\n";
                exit(1);
            }
        }
        else {
            cerr<<"invalid usage\n";
            exit(1);
        }
    }
    if(time_slice<2){
        cerr<<"too small time slice\n";
        exit(1);
    }

    if(fin.is_open()){
        string tmp_name;
        while(getline(fin, tmp_name)){
            process proc(tmp_name, 0, global_t);
            shptr->proc_queue.push_back(proc);
        }
    }
    fin.close();
    vector<pid_t> processors;
    size_t num_pros=shptr->proc_queue.size();
    size_q=(num_pros/num_processors)+1;

    for(size_t i=0; i<num_processors; ++i){
        pid_t pid=fork();
        if(pid==0){
            choose_policy(shptr);
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



void choose_policy(shared_data*& shptr){
    switch(pol){
    case FCFS:
        fcfs(shptr);
        break;
    case RR:
        rr(shptr);
        break;
    case RRAFF:
        rraff(shptr);
        break;
    }
    exit(0);
}
