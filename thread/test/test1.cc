#include <iostream>
#include <ctime>
#include <unistd.h>
#include "thread/thread.h"
using std::cout;
using std::endl;

void thread_func1() {
    time_t begin = time(nullptr);
    cout << yxalp::CureentThread::name() << endl;
    cout << yxalp::CureentThread::tid() << endl;
    while (time(nullptr) - begin < 50) {
        // 多个线程使用同一个输出流, 在 << 操作符中间有可能被中断, 比如 << endl; 之前
        cout << "thread_func1" << endl;
        sleep(1);
    }
}

void thread_func2() {
    time_t begin = time(nullptr);
    cout << yxalp::CureentThread::name() << endl;
    cout << yxalp::CureentThread::tid() << endl;
    while (time(nullptr) - begin < 50) {
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

    // cout << thread1.name() << endl;
    // 未实现
    // cout << yxalp::CureentThread::name() << endl;
    // cout << yxalp::CureentThread::tid() << endl;
    thread1.Join();
    thread2.Join();
}
