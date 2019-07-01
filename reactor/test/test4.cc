#include "../eventhandler.h"
#include "../dispatchthread.h"

#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>
#include <unistd.h>

void timeout() {
    printf("Timeout!\n");
    printf(" name : %s ,tid : %lu\n", yxalp::CureentThread::name(), yxalp::CureentThread::tid());
}

int main() {
    yxalp::DispatchThread dt;

    yxalp::Dispatcher *dispatcher =  dt.Start();
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    yxalp::EventHandler eh(timerfd, dispatcher);  // 跨线程, 不正确, 只做测试
    printf("timerfd: %d\n", timerfd);
    eh.set_read_func(&timeout);
    eh.set_care_read();

    struct itimerspec howlong;  // 传递给 timerfd 的结构体
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    while (true) {
        // loop
        sleep(1);
    }
    // 
    close(timerfd);
    return 0;
}
