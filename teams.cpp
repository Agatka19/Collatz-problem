#include <utility>
#include <deque>
#include <future>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <functional>
#include <thread>
#include <sys/wait.h>

#include "teams.hpp"
#include "contest.hpp"
#include "collatz.hpp"

using namespace std;

void new_th_collatz(InfInt input, ContestResult &r, uint32_t j) {
    r[j] = calcCollatz(input);
}


void const_th_collatz(ContestInput const & input, ContestResult &r, uint64_t per_thread, uint32_t start, size_t contest_size) {
    for(uint64_t i = start; i < contest_size; i += per_thread) {
        uint64_t res = calcCollatz(input.at(i));
        r[i] = res;
    }
}

ContestResult TeamNewThreads::runContestImpl(ContestInput const & contestInput)
{
    size_t contest_size = contestInput.size();
    ContestResult r(contest_size);
    uint32_t no_threads = getSize();
    thread threads[no_threads];
    
    uint32_t i = 0, j, k = 0;
    while(i < contest_size) {
        for(j = 0; j < no_threads && i < contest_size; ++j) {
            InfInt input = contestInput.at(i);
            thread t = createThread([input, &r, i] { new_th_collatz(input, r, i); });
            threads[j] = move(t);
            ++i;
        }

        for(j = 0; j < no_threads && k < contest_size; ++j) {
            threads[j].join();
            ++k;
        }
    }
    return r;
}


ContestResult TeamConstThreads::runContestImpl(ContestInput const & contestInput)
{
    size_t contest_size = contestInput.size();
    ContestResult r(contest_size);
    uint32_t no_threads = getSize();
    thread threads[no_threads];
    uint32_t start;

    for(uint32_t i = 0; i < no_threads; ++i) {
        thread t = createThread([contestInput, &r, no_threads, i, contest_size] { const_th_collatz(contestInput, r, no_threads, i, contest_size); });
        threads[i] = move(t);
    }
    
    for(uint32_t i = 0; i < no_threads; ++i) {
        threads[i].join();
    }
    return r;
}

ContestResult TeamPool::runContest(ContestInput const & contestInput)
{
    ContestResult r;
    size_t contest_size = contestInput.size();
    for(size_t i = 0; i < contest_size; ++i) {
        r.push_back(pool.push(calcCollatz, contestInput.at(i)).get());
    }
    return r;
}

ContestResult TeamNewProcesses::runContest(ContestInput const & contestInput)
{
    //TODO
    ContestResult r;
    return r;
}

ContestResult TeamConstProcesses::runContest(ContestInput const & contestInput)
{
    //TODO
    ContestResult r;
    return r;
}

ContestResult TeamAsync::runContest(ContestInput const & contestInput)
{
    ContestResult r;
    size_t contest_size = contestInput.size();
    for(size_t i = 0; i < contest_size; ++i) {
        future<uint64_t> res = async(calcCollatz, contestInput.at(i));
        r.push_back(res.get());
    }
    return r;
}
