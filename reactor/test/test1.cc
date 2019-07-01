#include "../eventhandler.h"
#include "../dispatcher.h"

#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>
#include <unistd.h>

yxalp::Dispatcher* global_dispatcher;

void timeout() {
    printf("Timeout!\n");
    global_dispatcher->Quit();
}

int main() {
    yxalp::Dispatcher dispatcher;
    global_dispatcher = &dispatcher;

    // 单次触发的定时器. 
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    // 所以用户主要是使用 channel 注册自己的回调函数, 
    yxalp::EventHandler eh(timerfd, &dispatcher);
    //   dispatcher.RegisterEventHandler(&eh);
    printf("timerfd: %d\n", timerfd);
    // dispatcher.RegisterEventHandler(&eh);  // 到 Dispatch 上注册自己
    eh.set_read_func(&timeout);
    eh.set_care_read();

    struct itimerspec howlong;  // 传递给 timerfd 的结构体
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 3;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    dispatcher.Loop();

    close(timerfd);
    return 0;
}
