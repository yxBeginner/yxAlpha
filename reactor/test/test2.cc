#include "../eventhandler.h"
#include "../dispatcher.h"

#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>
#include <unistd.h>

yxalp::Dispatcher* global_dispatcher;

void timeout1()
{
    printf("Timeout 1 !\n");
    // global_dispatcher->Quit();
}

void timeout2()
{
    printf("Timeout 2 !\n");
    global_dispatcher->Quit();
}

int main() {
    yxalp::Dispatcher dispatcher;
    global_dispatcher = &dispatcher;

    int timerfd1 = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    yxalp::EventHandler eh1(timerfd1, &dispatcher);
    eh1.set_read_func(&timeout1);
    eh1.set_care_read();

    struct itimerspec howlong;  // 传递给 timerfd 的结构体
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 3;
    ::timerfd_settime(timerfd1, 0, &howlong, NULL);

    // 2
    int timerfd2 = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    yxalp::EventHandler eh2(timerfd2, &dispatcher);
    eh2.set_read_func(&timeout2);
    eh2.set_care_read();

    struct itimerspec howlong2;  // 传递给 timerfd 的结构体
    bzero(&howlong2, sizeof howlong2);
    howlong2.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd2, 0, &howlong2, NULL);

    dispatcher.Loop();

    close(timerfd1);
    close(timerfd2);
    return 0;
}
