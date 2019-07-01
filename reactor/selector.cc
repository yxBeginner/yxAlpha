// yx

#include "selector.h"

#include <sys/epoll.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
// #include <iostream>  // debug

#include "eventhandler.h"

namespace yxalp {

Selector::Selector() : e_events_(kEEventsSize) {
    // epoll_create();  // abandon
    epollfd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epollfd_ < 0) {
        // log
    }
}

Selector::~Selector() {
    close(epollfd_);
}

void Selector::Select(int timeout, vector<EventHandler*> *handlers) {
    int ret = epoll_wait(epollfd_, e_events_.data(), static_cast<int>(e_events_.size()), timeout);
    // std::cout << "active nums : " << ret << std::endl;
    if (ret > 0) {  // triggered
        if (static_cast<size_t> (ret) == e_events_.size()) {  // 暂时没有缩小动作
            e_events_.resize(e_events_.size() * 2);  // 这种扩容方法在水平触发下才可以
        }
        UpdataTriggeredEventHandlers(ret, handlers);
    } else if (ret == -1) {
        // log epoll_wait error.
    } else {
        // nothing happened.
    }
}

void Selector::UpdataTriggeredEventHandlers(int nums, vector<EventHandler*> *handlers) {
    assert(static_cast<size_t> (nums) <= e_events_.size());
    EventHandler * tmp = nullptr;
    for (int i = 0; i < nums; ++i) {
        tmp = static_cast<EventHandler *> (e_events_[i].data.ptr);
        tmp->set_revents(e_events_[i].events);
        handlers->push_back(tmp);  // 反馈至 Dispatcher, 由后者调用回调函数
    }
}

void Selector::UpdataOneEventHandler(int option, EventHandler * ehandler) {
    // assert(option == (ADD||DEL||MOD));
    assert(ehandler != nullptr);
    switch (option) {
    case ADD:
        // std::cout << "call add" << std::endl;
        AddEvent(ehandler);
        break;
    case DEL:
        // std::cout << "call delete" << std::endl;
        DeleteEvent(ehandler);
        break;
    case MOD:
        // std::cout << "call mod" << std::endl;
        ModifyEvent(ehandler);
        break;
    default:
        // LOG error option
        break;
    }
}

void Selector::AddEvent(EventHandler * eh) {
    struct epoll_event e_event;
    bzero(&e_event, sizeof(struct ::epoll_event));
    int fd = eh->fd();
    // std::cout << "fffffffdddd " << fd<< std::endl;
    // std::cout << "ptr" << &eh<< std::endl;
    e_event.data.fd = fd;
    e_event.data.ptr = static_cast<void *> (eh);  // 使用 ptr 保存其所在 EventHandler
    e_event.events = eh->events();
    // std::cout << "epoll in add " << e_event.data.fd<< std::endl;
    // std::cout << "epoll in add " << e_event.data.ptr << std::endl;    
    // std::cout << "epoll in add " << e_event.events << std::endl;    
    int ret = epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &e_event);  // 也许可以把普通的 read update 直接合并到一起
    if (ret < 0) {
        // std::cout << "error in add " << ret << std::endl;
        // log error in add.
    }
}

void Selector::DeleteEvent(EventHandler * eh) {
    struct epoll_event e_event;
    bzero(&e_event, sizeof(struct ::epoll_event));
    int fd = eh->fd();
    e_event.data.fd = fd;
    e_event.data.ptr = static_cast<void *> (eh);
    e_event.events = eh->events();
    int ret = epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &e_event);
    if (ret < 0) {
        // log error in delete.
    }
}

void Selector::ModifyEvent(EventHandler * eh) {
    struct epoll_event e_event;
    bzero(&e_event, sizeof(struct ::epoll_event));
    int fd = eh->fd();  // 将此项设置为 -1, 可以让 epoll 暂时忽略该 fd 吗? poll 可以.
    e_event.data.fd = fd;
    e_event.data.ptr = static_cast<void *> (eh);
    e_event.events = eh->events();
    int ret = epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &e_event);
    if (ret < 0) {
        // std::cout << "error in mod " << ret << std::endl;
        // log error in modify.
    }
}

}  // namespace yxalp