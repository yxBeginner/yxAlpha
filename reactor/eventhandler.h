// yx
#ifndef YXALPHA_EVENTHANDLER_H_
#define YXALPHA_EVENTHANDLER_H_

#include <functional>
#include<iostream>

namespace yxalp {

class Dispatcher;

class EventHandler {
public:
    typedef std::function<void()> EventFunc;
    // fd 为监听的文件描述符, 但是 fd 的生命周期并不由 EventHandler 管理
    EventHandler(int fd, Dispatcher  *dp);
    ~EventHandler();
    EventHandler(const EventHandler &) = delete;
    EventHandler & operator=(const EventHandler &) = delete;

    void HandleEvent();
    void set_read_func(const EventFunc &func) { read_func_ = func;  }
    void set_write_func(const EventFunc &func) { write_func_ = func;  }
    void set_error_func(const EventFunc &func) { error_func_ = func;  }
    void set_close_func(const EventFunc &func) { close_func_ = func;  }

    // seletor 使用
    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }

    void set_care_read() { events_ |= kRead; change_interest(); }
    void set_care_write() { events_ |= kWrite; change_interest(); }
    void set_stop_care_read() { events_ &= ~kRead; change_interest(); }
    void set_stop_care_write() { events_ &= ~kWrite; change_interest(); }
    void set_stop_care_all() { events_ &= kNone; change_interest(); }
    bool is_care_nothing() { return events_ == kNone; }
    bool is_care_write() { return events_ & kWrite; }

private:
    void change_interest();  // 告知 Dispatcher 自己关注的事件改变
    static const int kRead;
    static const int kWrite;
    static const int kNone = 0;
    bool event_handling_;  // 标识正在处理 EventHandler

    EventFunc read_func_;
    EventFunc write_func_;
    EventFunc error_func_;
    EventFunc close_func_;

    const int fd_;
    int events_;
    int revents_;
    Dispatcher *dispatcher_;
};  // EventHandler

}  // yxalp

# endif  // YXALPHA_EVENTHANDLER_H_
