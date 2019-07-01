// yx
#ifndef YXALPHA_EVENTHANDLER_H_
#define YXALPHA_EVENTHANDLER_H_

#include <functional>

namespace yxalp {

class Dispatcher;
// Event Handler 不管理 fd 的生命周期
// Event Handler：定义事件处理方法：handle_event()，以供InitiationDispatcher回调使用。
// channel 只隶属于一个 IO 线程, 非跨线程
// 事件处理器，其会定义一些回调方法或者称为钩子函数，
// 当handle上有事件发生时，回调方法便会执行，一种事件处理机制。
// EventHandler 保存 fd 上关系的事件
// EventHanlder 不用于派生 Concrete EventHandler, 而是通过设置 function 的方式提供多态
class EventHandler {
public:
    typedef std::function<void()> EventFunc;
    // fd 为管理的文件描述符, 但是 fd 的生命周期并不由 EventHandler 管理
    // Dispatcher 构造 EventHandler 后, 需要注册自己, 以便 EventHandler 改变
    // 自己关心的事件. 考虑到 Dispatcher 不会正常退出, 那么这个引用是安全的(question).
    EventHandler(int fd, Dispatcher  *dp);
    ~EventHandler();
    EventHandler(const EventHandler &) = delete;
    EventHandler & operator=(const EventHandler &) = delete;

    void HandleEvent();
    void set_read_func(const EventFunc &func) { read_func_ = func;  }
    void set_write_func(const EventFunc &func) { write_func_ = func;  }
    void set_error_func(const EventFunc &func) { error_func_ = func;  }

    // seletor 使用
    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }

    void set_care_read() { events_ |= kRead; change_interest(); }
    void set_care_write() { events_ |= kWrite; change_interest(); }
    void set_stop_care_read() { events_ &= ~kRead; change_interest(); }
    void set_stop_care_write() { events_ &= ~kWrite; change_interest(); }

private:
    void change_interest();  // 告知 Dispatcher 自己关注的事件改变
    static const int kRead;
    static const int kWrite;

    EventFunc read_func_;
    EventFunc write_func_;
    EventFunc error_func_;

    // struct pollfd
    const int fd_;
    int events_;
    int revents_;
    Dispatcher *dispatcher_;
};  // EventHandler

}  // yxalp

# endif  // YXALPHA_EVENTHANDLER_H_
