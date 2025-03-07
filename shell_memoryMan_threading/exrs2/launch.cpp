#include "ipc_utils.h"
using namespace std;
#define MAX_PASS 80
#define MAX_BOAT 20
void initialize();
void join();
void working_pass();
void working_boat();

extern int psaved, boats_num, max_cap, pass_num;
extern counting_semaphore<INT_MAX> sem_seat;

int main(){
    do{
        cout<<"Insert pasengerNumber\n";
        cin>>pass_num;
        cout<<"Insert  boatsNumber\n";
        cin>>boats_num;
        cout<<"Insert max capacity\n";
        cin>>max_cap;
    }while(pass_num>MAX_PASS || boats_num>MAX_BOAT);
    sem_seat.release(boats_num*max_cap);
    initialize();
    join();
    cout<<"All passengers are now safe\n";
}



void initialize(){
    passenger* ps;
    boat* bt;
    for(int j=0; j<pass_num; j++){
        passenger::pass_queue.emplace_back(j);
    }
    for(int j=0; j<boats_num; j++){
        boat::boats.emplace_back(j);
    }
}

void join(){
    working_boat();
    working_pass();
}


void working_pass(){
    for(auto& ps : passenger::pass_queue){
        ps.join();
    }
}

void working_boat(){
    for(auto& bt : boat::boats){
        bt.join();
    }
}

