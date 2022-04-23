#ifndef SHAREDRESULTS_HPP
#define SHAREDRESULTS_HPP

#include <thread>
#include <mutex>

#define MAX_SIZE 1000000

using namespace std;


class SharedResults
{
    public:

    static InfInt RESULTS[MAX_SIZE];
    mutex mtx;

    void set(InfInt n, int place) {
        lock_guard<mutex> guard(mtx);
        RESULTS[place] = n;
    }

    InfInt get(int place) {
        lock_guard<mutex> guard(mtx);
        return RESULTS[place];
    }

};


#endif