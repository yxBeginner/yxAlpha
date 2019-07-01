#include "../dispatcher.h"
#include <stdio.h>

yxalp::Dispatcher *global_dispatcher;
int g_flag = 0;

void run4() {
    printf("run4(): tid = %lu, flag = %d\n", pthread_self(), g_flag);
    global_dispatcher->Quit();
}

void run2() {
  printf("run2(): tid = %lu, flag = %d\n", pthread_self(), g_flag);
  global_dispatcher->QueueInLoop(run4);
    // 如果处理用户任务回调, 将 ExecutePendingFuncs 放到
    // handleRead 中, 则需要手动调用一次 wakeup
    global_dispatcher->Wakeup();
}

void run1() {
    g_flag = 1;
    printf("run1(): tid = %lu, flag = %d\n", pthread_self(), g_flag);
    global_dispatcher->RunInLoop(run2);  // 在任务回调中, 添加新回调
    g_flag = 2;  // 回调返回后修改, run2中看到的依然是 1
}

int main() {
    printf("main(): tid = %lu, flag = %d\n", pthread_self(), g_flag);

    yxalp::Dispatcher dispatcher;
    global_dispatcher = &dispatcher;

    dispatcher.RunInLoop(run1);
    dispatcher.Loop();
    printf("main(): tid = %lu, flag = %d\n", pthread_self(), g_flag);
}
