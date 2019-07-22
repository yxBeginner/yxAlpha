// yx
#include "eventhandler.h"

#include <sys/epoll.h>
#include "dispatcher.h"
#include "logger/logging.h"

namespace yxalp {

// const int EventHandler::kRead = EPOLLIN|EPOLLET;  // edge trigger
const int EventHandler::kRead = EPOLLIN;  // level trigger
const int EventHandler::kWrite = EPOLLOUT;

EventHandler::EventHandler(int fd, Dispatcher  *dp)
    : event_handling_(false),
      read_func_(nullptr),
      write_func_(nullptr),
      error_func_(nullptr),
      fd_(fd),
      events_(0),
      revents_(0),
      dispatcher_(dp) {
    dispatcher_->RegisterEventHandler(this);  // 是在构造的时候直接注册, 还是手动注册?
    
}

EventHandler::~EventHandler() {
    assert(!event_handling_);  // 只有所有回调执行完毕后, 才能析构
    DLOG << "EventHandler::~EventHandler() : "  << fd_;
    // dispatcher_->RemoveEventHandler(this);
}

void EventHandler::change_interest() {
    dispatcher_->UpdateEventHandler(this);
}

void EventHandler::HandleEvent() {
    // HandleEvent 过程中, 整个回调栈中不可以析构本对象
    event_handling_ = true;
    // peer 关闭 && 无数据可读
    if ((revents_ & (EPOLLHUP)) && !(revents_ & EPOLLIN )) {
        DLOG << "EventHandler::HandleEvent()  EPoll HUP";
        if (close_func_) {
            close_func_();
        }
    }
    if (revents_ & (EPOLLERR)) {
        LOG << "EventHandler::HandleEvent()  EPoll Error";
        if (error_func_) {
            error_func_();
        } else {}
    }
    // 可读 | 急迫数据可读(带外数据) | 关闭/半关闭
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (read_func_) {
            read_func_();
        } else {
            // log
        }
    }
    // 写数据不会阻塞
    if (revents_ & EPOLLOUT) {
        if (write_func_) {
            write_func_();
        } else {
            // log
        }
    }
    event_handling_ = false;
}

};  // namespace yxalp
