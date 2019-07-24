// yx
#ifndef YXALPHA_DISPATCHER_H_
#define YXALPHA_DISPATCHER_H_

#include <vector>
#include <memory>
#include <functional>
#include <atomic>

#include "thread/thread.h"
#include "thread/mutex.h"

// dispatcher 与 selector 皆不维护 EventHandler 的列表, 由 kernel 维护
namespace yxalp {
class EventHandler;
class Selector;

// 由 IO 线程创建, 一对一关系
class Dispatcher {
public:
    Dispatcher();
    ~Dispatcher();
    Dispatcher(const Dispatcher &) = delete;
    Dispatcher & operator=(const Dispatcher &) = delete;

    // 不可跨线程
    void Loop();
    // 可被其他线程调用
    void Quit();
    // 可跨线程间调配任务
    void RunInLoop(const std::function<void ()> &func);

    void QueueInLoop(const std::function<void ()> &func);

    void Wakeup();

    void RegisterEventHandler(EventHandler * ehandler);
    void RemoveEventHandler(EventHandler * ehandler);
    void UpdateEventHandler(EventHandler * ehandler);

    // 在需要断言不可跨线程的调用时使用
    void AssertInLoopThread();

    bool is_same_thread() const {
        return static_cast<bool> (tid_  == CureentThread::tid());
    }

    const std::atomic_bool & is_looping() const { return looping_; }

private:
    // wakeup_handler 的 read 事件回调
    void HandleRead();
    void ExecutePendingFuncs();  // 处理

    static const int kEpollTimeOut = 1000;
    std::atomic<bool> looping_;
    std::atomic<bool> quit_;  // 正常退出, 跨线程对象, 需要保证原子性
    tid_t tid_;  // 创建者/拥有者线程 (一对一)
    std::unique_ptr<Selector> selector_;
    /* 以下与唤醒本 Dispatcher, 并处理用户添加任务回调相关 */
    int wakeup_fd_;
    bool in_calling_pending_funcs_;  // 表示正在处理用户添加回调函数
    std::unique_ptr<EventHandler> wakeup_handler_;  // 用于唤醒
    MutexLock mutex_;  // guard 任务回调函数队列
    std::vector<std::function<void()> > pending_funcs_;
};

}  // namespace yxalp

# endif  // YXALPHA_DISPATCHER_H_
