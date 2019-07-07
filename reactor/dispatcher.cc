// yx
#include "dispatcher.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <cassert>

#include "selector.h"
#include "eventhandler.h"
#include "logger/logging.h"

namespace yxalp {

// 创建一个 eventfd
static int CreateEventFd() {
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0) {
        LOG << "CreateEventFd() : Error " << fd;
        abort();  // dump
    }
    return fd;
}

Dispatcher::Dispatcher()
    : looping_(false), 
      quit_(false),
      tid_(CureentThread::tid()),
      selector_(std::make_unique<Selector>()),
      event_handler_list_(),
      wakeup_fd_(CreateEventFd()),
      in_calling_pending_funcs_(false),
      wakeup_handler_(std::make_unique<EventHandler> (wakeup_fd_, this)),
      mutex_(),
      pending_funcs_() {
    LOG << "Dispatcher::Dispatcher() : Constructor.";
    wakeup_handler_->set_read_func(std::bind(&Dispatcher::HandleRead, this));
    wakeup_handler_->set_care_read();
}

Dispatcher::~Dispatcher() {
    LOG << "Dispatcher::Dispatcher() : Destructor.";
    assert(!looping_);
    close(wakeup_fd_);
}

void Dispatcher::Quit() {
    assert(!quit_);
    quit_ = true;
    if (!is_same_thread()) {
        Wakeup();  // 唤醒以退出
    }
}

void Dispatcher::RegisterEventHandler(EventHandler * ehandler) {
    DLOG << "Dispatcher::RegisterEventHandler : epoll in add : " << ehandler->fd();
    // std::cout << "epoll in add " << ehandler->events() << std::endl;    
    selector_->UpdataOneEventHandler(Selector::ADD, ehandler);
}

void Dispatcher::RemoveEventHandler(EventHandler * ehandler) {
    DLOG << "Dispatcher::RemoveEventHandler : epoll in delete : " << ehandler->fd();
    selector_->UpdataOneEventHandler(Selector::DEL, ehandler);
}

void Dispatcher::UpdateEventHandler(EventHandler * ehandler) {
    DLOG << "Dispatcher::UpdateEventHandler : epoll in modify : " << ehandler->fd();
    selector_->UpdataOneEventHandler(Selector::MOD, ehandler);
}

void Dispatcher::AssertInLoopThread() {
    if (!is_same_thread()) {
        DLOG << "Dispatcher::AssertInLoopThread() : threadId : " << tid_
                  << " Current thread id : " << CureentThread::tid()
                  << " with name : " << CureentThread::name();
        abort();
    }
}

void Dispatcher::Loop() {
    assert(!looping_);  // Loop 开始
    AssertInLoopThread();
    assert(!quit_);
    looping_ = true;
    quit_ = false;
    while (!quit_) {
        event_handler_list_.clear();  // C11 释放空间
        selector_->Select(kEpollTimeOut, &event_handler_list_);
        LOG << "Dispatcher::Loop() : one round epoll" 
                  << " have active ? " << !event_handler_list_.empty();
        for (auto it = event_handler_list_.cbegin(); it != event_handler_list_.cend(); ++it) {
            // std::cout << "active" << &it << std::endl;
            (*it)->HandleEvent();
        }
        ExecutePendingFuncs();  // 执行异步用户任务回调
    }
    LOG << "Dispatcher::Loop() : stop looping in thread id : " << tid_
              << " with name : " << CureentThread::name();
    looping_ = false;
}

void Dispatcher::RunInLoop(const std::function<void ()> &func) {
    if (is_same_thread()) {
        func();  // 本线程内添加新任务, (只可能是 Loop 未开始之前)
    } else {
        QueueInLoop(func);
    }
}

void Dispatcher::QueueInLoop(const std::function<void ()> &func) {
    {  // 临界区
        MutexLockGuard lock_up(mutex_);
        pending_funcs_.push_back(func);
    }  // 临界区
    // 注意 wakeup 的条件, 当本线程正在处理用户任务回调时, 需要 wakeup
    // 以期望在下一轮 epoll 时, 可以立即返回
    if (!is_same_thread() || in_calling_pending_funcs_) {
        Wakeup();
    }
}

// 向 eventfd 写入一字节以 wake up 本线程
void Dispatcher::Wakeup() {
    const char *wakeup_buf {"wake up"};
    ssize_t ret = write(wakeup_fd_, &wakeup_buf, sizeof(wakeup_buf));
    if (ret < 0) {
        LOG << "Dispatcher::Wakeup()  : write eventfd error thread id : " << tid_
                  << " with name : " << CureentThread::name();
    }
}

// 读出 wakeup 时写入的一个字节, EventHandler 的回调函数
// 不在 wakeup 的 HandleRead 中处理 Functors
void Dispatcher::HandleRead() {
    char wakeup_buf[8];
    // eventfd_read()
    ssize_t ret = read(wakeup_fd_, &wakeup_buf, sizeof(wakeup_buf));
    if (ret < 0) {
        LOG << "Dispatcher::HandleRead()  : read eventfd error thread id : " << tid_
                  << " with name : " << CureentThread::name();
    }
}

void Dispatcher::ExecutePendingFuncs() {
    std::vector<std::function<void()> > local_pending_funcs;
    in_calling_pending_funcs_ = true;  // Atomic
    // swap, 减小临界区
    {  // 临界区
        MutexLockGuard lock_up(mutex_);
        local_pending_funcs.swap(pending_funcs_);
    }  // 临界区
    // swap, 在交换后的 functors 上遍历, 否则, 用户回调中调用了 QueueInLoop
    // 将导致重复上锁.
    for (auto &func : local_pending_funcs) {
        func();
    }
    // 注意该 bool 表达式不是为了告知其他线程本线程正在处理 functors,
    // 那是一个条件变量, 会阻塞其他线程, 不是我们想要的.
    in_calling_pending_funcs_ = false;
}

}  // yxalp
