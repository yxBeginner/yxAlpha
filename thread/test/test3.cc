#include <iostream>
#include <ctime>
#include <unistd.h>
#include "thread/threadpool.h"
using std::cout;
using std::endl;

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

int main() {

    yxalp::ThreadPool tp(10);
    time_t begin = time(nullptr);
    sleep(5);
    tp.Start(5);
    int i = 0;
    while (time(nullptr) - begin < 30) {
        if (i % 2 != 0) {
            tp.PushTask(thread_func1);
        } else {
            tp.PushTask(thread_func3);
        }
        ++i;
        // sleep(1);
    }
    tp.Stop();
    return 0;
}
