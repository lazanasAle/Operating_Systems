#include "ipc_utils.h"
#include <climits>
#include <chrono>
using namespace std::chrono;

int psaved=0, boats_num, max_cap, pass_num;
counting_semaphore<INT_MAX> sem_pass(0), sem_seat(0);
mutex mtx1,  mtx_print2;
list<passenger> passenger::pass_queue;
list<boat> boat::boats;


//Functions for passenger
passenger::passenger(int id){
    this->id=id;
    wants=true;
    th=new thread(&passenger::get_boat, this);
}



void passenger::get_boat(){
    sem_seat.acquire();
    mtx1.lock();
    psaved++;
    sem_pass.release();
    mtx1.unlock();
    reach_coast();
}

void passenger::reach_coast(){
    ostringstream oss;
    oss<<"Passenger: "<<id<<" reached the coast\n";
    mtx_print2.lock();
    cout<<oss.str();
    mtx_print2.unlock();
}

void passenger::join() const{
    if(th->joinable())
        th->join();
}

passenger::~passenger(){
    delete th;
}



//Functions for boat
boat::boat(int id){
    this->id=id;
    th=new thread(&boat::get_passenger, this);
}

void boat::get_passenger(){
    bool trip_done=false;
    while(true){
        mtx1.lock();
        if(psaved>=pass_num){
            mtx1.unlock();
            break;
        }
        else
            mtx1.unlock();
        int j=0;
        while(true){
            mtx1.lock();
            if((j>=max_cap) || (psaved>=pass_num)){
                mtx1.unlock();
                break;
            }
            else
                mtx1.unlock();
            sem_pass.acquire();
            j++;
        }
        mtx1.lock();
        if(j>=min(max_cap, pass_num-psaved)){
            mtx1.unlock();
            trip_done=true;
            reach_coast();
        }
        else
            mtx1.unlock();
        mtx1.lock();
        int remaining=min(max_cap, pass_num-psaved);
        mtx1.unlock();
        sem_seat.release(remaining);
    }
    if(!trip_done)
        reach_coast();
}

void boat::reach_coast(){
    ostringstream oss;
    oss<<"Boat: "<<id<<" reached coast\n";
    this_thread::sleep_for(milliseconds(14));
    mtx_print2.lock();
    cout<<oss.str();
    mtx_print2.unlock();
}

void boat::join() const{
    if(th->joinable())
        th->join();
}

boat::~boat(){
    delete th;
}





