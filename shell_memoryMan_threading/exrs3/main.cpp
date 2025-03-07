#include "memory_utils.cpp"
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <list>
#include <chrono>
#define TIME_SLICE 3
using namespace std;

list<process> all_process;
list<process> pqueue;
b_vector memory;
size_t num_process;
void initialize();
void Rrobin();

size_t curr_time;

int main(){
    initialize();
    Rrobin();
}

void initialize(){
    string fname = "processes.txt";
    ifstream process_file(fname);

    if(process_file.is_open()){
        string line;
        process* proc;
        while(getline(process_file, line)){
            vector<string> row;
            stringstream ss(line);
            string cell;
            while(getline(ss, cell, ',')) {
                row.push_back(cell);
            }
            proc = new process(stoi(row[0]), stoi(row[1]), stoi(row[2]), stoi(row[3]));
            all_process.push_back(*proc);
        }
        num_process=all_process.size();
    }
}


void Rrobin(){
    auto executable = all_process.front();
    if(executable.arival_time<=curr_time){
        all_process.pop_front();
        pqueue.push_back(executable);
    }
    else if(pqueue.empty()){
        this_thread::sleep_for(chrono::milliseconds(executable.arival_time));
        curr_time+=executable.arival_time;
        all_process.pop_front();
        pqueue.push_back(executable);
    }
    label:
    while(num_process>0){
        auto to_exec = pqueue.front();
        pqueue.pop_front();
        size_t exec_time = min((size_t)TIME_SLICE, to_exec.remaining_time);
        if(to_exec.remaining_time==to_exec.duration){
            if(memory.get_proc(to_exec)){
                to_exec.remaining_time-=exec_time;
                memory.print_vector();
            }
            else{
                pqueue.push_back(to_exec);
                cerr<<"memory not enough for process: "<<to_exec.pid<<" proceeding to the next\n";
                goto label;
            }
        }
        else{
            to_exec.remaining_time-=exec_time;
        }
        cout<<"executing "<<exec_time<<" of process: "<<to_exec.pid<<endl;
        this_thread::sleep_for(chrono::seconds(exec_time));
        curr_time+=exec_time;
        while(!all_process.empty() && all_process.front().arival_time<=curr_time){
            auto ready = all_process.front();
            all_process.pop_front();
            pqueue.push_back(ready);
        }
        if(to_exec.remaining_time<=0){
            memory.remove_proc(to_exec);
            num_process--;
            memory.print_vector();
        }
        else{
            pqueue.push_back(to_exec);
        }
    }
}
