// 压测, log 中有关时间的系统调用非常耗时, 性能会下降 80%
#include <ctime>
#include <unistd.h>
#include "thread/thread.h"
#include "../logging.h"

void thread_func1() {
    time_t begin = time(nullptr);
    while (time(nullptr) - begin < 10) {
        LOG << yxalp::CureentThread::name() << " " << yxalp::CureentThread::tid();
        // sleep(1);
    }
}

void thread_func2() {
    time_t begin = time(nullptr);
    while (time(nullptr) - begin < 10) {
        LOG << yxalp::CureentThread::name() << " " << yxalp::CureentThread::tid();
        // sleep(1);
    }
}

int main() {
    std::string();
    yxalp::Thread thread1(thread_func1, std::string("thread 1"));
    yxalp::Thread thread2(thread_func2, std::string("thread 2"));
    yxalp::Thread thread3(thread_func1, std::string("thread 3"));
    yxalp::Thread thread4(thread_func1, std::string("thread 4"));
    
    thread1.Start();
    thread2.Start();
    thread3.Start();
    thread4.Start();

    time_t begin = time(nullptr);
    while (time(nullptr) - begin < 10) {
        LOG << "main thread";
        // sleep(1);
    }

    thread1.Join();
    thread2.Join();
    thread3.Join();
    thread4.Join();
    return 0;
}
