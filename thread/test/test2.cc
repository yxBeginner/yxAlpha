#include <iostream>
#include <ctime>
#include <unistd.h>
#include "thread/thread.h"
#include "thread/mutex.h"
#include "thread/condition.h"
using std::cout;
using std::endl;

yxalp::MutexLock mutex;
yxalp::Condition cond(mutex);

void thread_func1() {
    time_t begin = time(nullptr);
    cout << yxalp::CureentThread::name() << endl;
    cout << yxalp::CureentThread::tid() << endl;
    while (time(nullptr) - begin < 50) {
        cond.wait();  // 执行一次
        cout << "thread_func1" << endl;
        sleep(1);
    }
}

void thread_func2() {
    time_t begin = time(nullptr);
    cout << yxalp::CureentThread::name() << endl;
    cout << yxalp::CureentThread::tid() << endl;
    while (time(nullptr) - begin < 50) {
        cond.wait();
        cout << "thread_func2" << endl;
        sleep(1);
    }
}

int main() {
    std::string();
    yxalp::Thread thread1(thread_func1, std::string("thread 1"));
    yxalp::Thread thread2(thread_func2, std::string("thread 2"));

    thread1.Start();
    thread2.Start();

    time_t begin = time(nullptr);
    while (time(nullptr) - begin < 5) {
        cout << "main thread" << endl;
        sleep(1);
    }

    cond.notifyAll();
    thread1.Join();
    thread2.Join();
}
