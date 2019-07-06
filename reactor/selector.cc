// yx

#include "selector.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <cassert>
#include <cstring>

#include "logger/logging.h"
#include "eventhandler.h"

namespace yxalp {

Selector::Selector() : e_events_(kEEventsSize) {
    // epoll_create();  // abandon
    epollfd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epollfd_ < 0) {
        LOG << "Selector::Selector() : epoll_create1 ERROR";
    }
}

Selector::~Selector() {
    close(epollfd_);
}

static void epoll_err (int err) {
        LOG << "Selector::Select : log epoll_wait error : " << err;
        switch (err) {
        case EINTR:
            break;
        case EINVAL:
            LOG << "Selector::Select : epoll_wait error : epfd is not an epoll file descriptor, "
                            "or maxevents is less than or equal to zero.";
            abort();
            break;
        case EFAULT:  // TODO
            break;
        case EBADF:
            // epfd is not a valid file descriptor.
            break;
        default:
            break;
        }
}

void Selector::Select(int timeout, vector<EventHandler*> *handlers) {
    int ret = epoll_wait(epollfd_, e_events_.data(), static_cast<int>(e_events_.size()), timeout);
    LOG << "Selector::Select () : active nums : " << ret;
    if (ret > 0) {  // triggered
        if (static_cast<size_t> (ret) == e_events_.size()) {  // TODO 缩小动作
            e_events_.resize(e_events_.size() * 2);  // 这种扩容方法在水平触发下才可以
        }
        UpdataTriggeredEventHandlers(ret, handlers);
    } else if (ret == -1) {
        epoll_err(errno);  // other
    } else {
        LOG << "Selector::Select() : nothing happened";
    }
}

void Selector::UpdataTriggeredEventHandlers(int nums, vector<EventHandler*> *handlers) {
    assert(static_cast<size_t> (nums) <= e_events_.size());
    EventHandler * tmp = nullptr;
    handlers->resize(nums);
    for (int i = 0; i < nums; ++i) {
        tmp = static_cast<EventHandler *> (e_events_[i].data.ptr);
        tmp->set_revents(e_events_[i].events);
        // Q: 选择在 Selector 中直接处理 EventHandler 的 HandEvent() ?
        handlers->at(i) = tmp;  // 反馈至 Dispatcher, 由后者调用回调函数
        // same as (*handlers)[i] = tmp;
    }
}

void Selector::UpdataOneEventHandler(int option, EventHandler * ehandler) {
    // assert(option == (ADD||DEL||MOD));
    assert(ehandler != nullptr);
    switch (option) {
    case ADD:
        AddEvent(ehandler);
        break;
    case DEL:
        DeleteEvent(ehandler);
        break;
    case MOD:
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
    LOG << "Selector::AddEvent : fd : " << fd 
              << " address : " << &eh; 
    e_event.data.fd = fd;
    e_event.data.ptr = static_cast<void *> (eh);  // 使用 ptr 保存其所在 EventHandler
    e_event.events = eh->events();
    // std::cout << "epoll in add " << e_event.data.fd<< std::endl;
    // std::cout << "epoll in add " << e_event.data.ptr << std::endl;    
    // std::cout << "epoll in add " << e_event.events << std::endl;    
    int ret = epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &e_event);  // 也许可以把普通的 read update 直接合并到一起
    if (ret < 0) {
        LOG << "Selector::AddEvent : error in add " << ret;
    }
}

void Selector::DeleteEvent(EventHandler * eh) {
    struct epoll_event e_event;
    bzero(&e_event, sizeof(struct ::epoll_event));
    int fd = eh->fd();
    LOG << "Selector::DeleteEvent : fd : " << fd 
              << " address : " << &eh;
    e_event.data.fd = fd;
    e_event.data.ptr = static_cast<void *> (eh);
    e_event.events = eh->events();
    int ret = epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &e_event);
    if (ret < 0) {
        LOG << "Selector::DeleteEvent : error in delete " << ret;
    }
}

// TODO if care nothing, 应当将 fd 设置为 -fd-1, 否则改变回来
void Selector::ModifyEvent(EventHandler * eh) {
    struct epoll_event e_event;
    bzero(&e_event, sizeof(struct ::epoll_event));
    int fd = eh->fd();  // 将此项设置为 -1, 可以让 epoll 暂时忽略该 fd 吗? poll 可以.
    LOG << "Selector::ModifyEvent : fd : " << fd 
              << " address : " << &eh;
    e_event.data.fd = fd;
    e_event.data.ptr = static_cast<void *> (eh);
    e_event.events = eh->events();
    int ret = epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &e_event);
    if (ret < 0) {
        LOG << "Selector::ModifyEvent : error in mod" << ret;
    }
}

}  // namespace yxalp