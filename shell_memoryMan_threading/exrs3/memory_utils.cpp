/************************************************************************************************************************************************************************************************************/
//Alexios Lazanas, 1100605
//Stamatia Kalimeri, 1103080
/************************************************************************************************************************************************************************************************************/

#include <vector>
#include <iostream>
#include <cmath>
#define MAX_SIZE 512
using namespace std;



class memory_block{
public:
    size_t start;
    size_t length;
    bool free;
    size_t* owner_pid;
    memory_block(int length, int j){
        free=true;
        this->length=length;
        owner_pid=nullptr;
        start=j;
    }
    memory_block(size_t owner_pid, int length, int j){
        free=false;
        this->owner_pid=new size_t;
        *(this->owner_pid)=owner_pid;
        this->length=length;
        start=j;
    }
    bool operator == (memory_block& other){
        return(*(this->owner_pid)==*(other.owner_pid));
    }
};


class process{
public:
    size_t pid;
    size_t arival_time;
    size_t duration;
    size_t remaining_time;
    size_t memory_needed;
    bool in_memory;
    process(){}
    process(size_t pid, size_t arival_time, size_t duration, size_t memory_needed){
        this->pid=pid;
        this->arival_time=arival_time;
        this->duration=duration;
        this->memory_needed=memory_needed;
        this->in_memory=false;
        this->remaining_time=duration;
    }
    ~process(){}

};


class b_vector{
    vector<memory_block> memory;
public:
    b_vector(){
        memory_block mb(MAX_SIZE, 0);
        memory.push_back(mb);
    }

    bool get_proc(process& proc){
        if(memory.size()>=MAX_SIZE){
            return false;
        }


        for(size_t j=0; j<memory.size(); ++j){
            if(memory[j].free && memory[j].length==proc.memory_needed){
                memory_block mb_proc(proc.pid, proc.memory_needed, j);
                memory[j]=mb_proc;
                return true;
            }
            else if(memory[j].free && memory[j].length>proc.memory_needed){
                memory_block mb_proc(proc.pid, proc.memory_needed, j);
                memory_block mb_free(memory[j].length-proc.memory_needed, j+1);
                memory.erase(memory.begin()+j);
                memory.insert(memory.begin()+j, mb_proc);
                memory.insert(memory.begin()+j+1, mb_free);
                return true;
            }
        }
        return false;
    }

    void remove_proc(process& proc){
        memory_block to_remove(proc.pid, 0, 0);

        for(size_t j=0; j<memory.size(); ++j){
            if((memory[j].owner_pid && to_remove.owner_pid) && (memory[j]==to_remove)){
                memory[j].free=true;
                memory[j].owner_pid=nullptr;
            }
        }
        auto it=memory.begin();
        int j=0;
        while(it!=memory.end()-1){
            if((*it).free && (*(it+1)).free){
                memory_block mb_new_b((*it).length+(*(it+1)).length, j);
                it=memory.erase(it);
                it=memory.erase(it);
                it=memory.insert(it, mb_new_b);

            }
            else{
                ++it;
            }
            ++j;
        }
    }

    void print_vector(){
        cout<<"[";
        for(memory_block& mb : memory){
            if(mb.owner_pid){
                cout<<"{"<<*mb.owner_pid<<"} ";
            }
            cout<<"("<<mb.length<<"),";
        }
        cout<<"]\n";
    }
};

