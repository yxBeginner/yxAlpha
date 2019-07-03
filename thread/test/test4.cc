#include <iostream>
#include <ctime>
#include <unistd.h>
#include "thread/threadpool.h"
using std::cout;
using std::endl;

yxalp::ThreadPool *tp;

void thread_func1() {
    cout << "thread_func1 " << yxalp::CureentThread::name()
              <<" "<< yxalp::CureentThread::tid() << endl;
        // sleep(1);
}

void thread_func2() {
    time_t begin = time(nullptr);
    cout << yxalp::CureentThread::name() << endl;
    cout << yxalp::CureentThread::tid() << endl;
    while (time(nullptr) - begin < 1) {
        cout << "thread_func2" << endl;
        sleep(1);
    }
}

void thread_func3() {
    cout << "thread_func3 " << yxalp::CureentThread::name()
              <<" "<< yxalp::CureentThread::tid() << endl;
        // sleep(1);
}

void thread_func4() {
    time_t begin = time(nullptr);
    while (time(nullptr) - begin < 30) {
            tp->PushTask(thread_func1);
            sleep(1);
        }
}

void thread_func5() {
    time_t begin = time(nullptr);
    while (time(nullptr) - begin < 30) {
            tp->PushTask(thread_func3);
            sleep(1);
        }
}

int main() {
    tp = new yxalp::ThreadPool(10);
    yxalp::Thread thread1(thread_func4, std::string("thread 1"));
    yxalp::Thread thread2(thread_func5, std::string("thread 2"));

    thread1.Start();
    thread2.Start();
    
    time_t begin = time(nullptr);
    sleep(5);
    // pool 启动之前, 由各个线程执行自己的任务.
    tp->Start(5);
    int i = 0;
    while (time(nullptr) - begin < 30) {
        if (i % 2 != 0) {
            tp->PushTask(thread_func1);
        } else {
            tp->PushTask(thread_func3);
        }
        ++i;
        sleep(1);
    }
    // tp->Stop();
    thread1.Join();
    thread2.Join();
    delete tp;
    return 0;
}
