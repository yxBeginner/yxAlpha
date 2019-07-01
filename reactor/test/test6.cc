// 跨线程测试添加任务回调, quit Loop 等功能
#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>
#include <unistd.h>

#include "../eventhandler.h"
#include "../dispatchthread.h"
#include "../dispatcher.h"

yxalp::Dispatcher *sec_dispatcher;
int g_flag = 0;

// 本来 10s 之后才会触发计时器事件
void timeout() {
    printf("Timeout!\n");
    printf(" name : %s ,tid : %lu\n", yxalp::CureentThread::name(), yxalp::CureentThread::tid());
    sec_dispatcher->Quit();
}

void run4() {
  printf("run4(): tid = %lu, flag = %d\n", pthread_self(), g_flag);
}

void run2() {
    printf("run2(): tid = %lu, flag = %d\n", pthread_self(), g_flag);
    sec_dispatcher->QueueInLoop(run4);
}

void run1() {
    g_flag = 1;
    printf("run1(): tid = %lu, flag = %d\n", pthread_self(), g_flag);
    sec_dispatcher->RunInLoop(run2);
    g_flag = 2;
}

int main() {
    yxalp::DispatchThread dt;
    sec_dispatcher =  dt.Start();
    
    // 单次定时器
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    yxalp::EventHandler eh(timerfd, sec_dispatcher);  // 跨线程, 不正确, 只做测试
    printf("timerfd: %d\n", timerfd);
    eh.set_read_func(&timeout);
    eh.set_care_read();

    struct itimerspec howlong;  // 传递给 timerfd 的结构体
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 10;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    // 从主线程向 Dispatcher 线程添加任务
    sec_dispatcher->RunInLoop(run1);

    // 在 sec thread 启动之前调用 quit 没有用的
    // sec_dispatcher->Quit();  // 在这里调用退出, 添加至 sec thread 的任务不一定执行完毕.
    while (true) {
        // loop
        printf("is looping ? %d\n", static_cast<int> (sec_dispatcher->is_looping()));
        sleep(1);
    }

    close(timerfd);
    return 0;
}
