// yx
#include "eventhandler.h"

#include <sys/epoll.h>
// #include <iostream>  // debug
#include "dispatcher.h"

namespace yxalp {

// const int EventHandler::kRead = EPOLLIN|EPOLLET;  // edge trigger
const int EventHandler::kRead = EPOLLIN;  // level trigger
const int EventHandler::kWrite = EPOLLOUT;

EventHandler::EventHandler(int fd, Dispatcher  *dp)
    : read_func_(nullptr),
      write_func_(nullptr),
      error_func_(nullptr),
      fd_(fd),
      events_(0),
      revents_(0),
      dispatcher_(dp) {
    // std::cout << fd << std::endl;
    dispatcher_->RegisterEventHandler(this);  // 是在构造的时候直接注册, 还是手动注册?
}

EventHandler::~EventHandler() {
    dispatcher_->RemoveEventHandler(this);
}

void EventHandler::change_interest() {
    dispatcher_->UpdateEventHandler(this);
}

void EventHandler::HandleEvent() {
    // std::cout << "HandleEvent" << std::endl;
    // // POLLNVAL 指定描述符非法
    // if (revents_ & EPOLLNVAL) {
    //     // log
    // }
    if (revents_ & (EPOLLERR)) {
        if (error_func_) {
            error_func_();
        } else {
            // log
        }
    }
    // 可读 | 急迫数据可读 | ?
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        // std::cout << "handle read" << std::endl;
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
}

};  // namespace yxalp
