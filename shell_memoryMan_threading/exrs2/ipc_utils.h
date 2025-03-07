#ifndef IPC_UTILS_H
#define IPC_UTILS_H
#include <iostream>
#include <list>
#include <thread>
#include <semaphore>
#include <mutex>
#include <sstream>
using namespace std;

class passenger{
public:
    static list<passenger> pass_queue;
    int id;
    bool wants;
    thread* th;

    passenger(int id);
    void get_boat();
    void reach_coast();
    void join() const;
    ~passenger();


};


class boat{
public:
    int id;
    thread* th;
    static list<boat> boats;

    boat(int id);
    void get_passenger();
    void reach_coast();
    void join() const;
    ~boat();
};



#endif // IPC_UTILS_H
