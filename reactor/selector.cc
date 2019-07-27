// yx
#include "selector.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <cassert>
#include <cstring>

#include "logger/logging.h"
#include "eventhandler.h"
#include "time/timestamp.h"

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
    case EINVAL: {
        LOG << "Selector::Select : epoll_wait error : epfd is not an epoll file descriptor, "
            "or maxevents is less than or equal to zero.";
        abort();
        break;
    }
    case EFAULT:  // TODO
        break;
    case EBADF:
        // epfd is not a valid file descriptor.
        break;
    default:
        break;
    }
}

Timestamp Selector::Select(int timeout) {
    int ret = epoll_wait(epollfd_, e_events_.data(), static_cast<int>(e_events_.size()), timeout);
    DLOG << "Selector::Select () : active nums : " << ret;
    Timestamp now(Timestamp::now());
    if (ret > 0) {  // triggered
        if (static_cast<size_t> (ret) == e_events_.size()) {
            e_events_.resize(e_events_.size() * 2);  // 这种扩容方法在水平触发下才可以
        }
        UpdataTriggeredEventHandlers(ret);
    } else if (ret == -1) {
        epoll_err(errno);  // other
    } else {
        DLOG << "Selector::Select() : nothing happened";
    }
    return now;  // or finish ?
}

void Selector::UpdataTriggeredEventHandlers(int nums) {
    assert(static_cast<size_t> (nums) <= e_events_.size());
    EventHandler * eh = nullptr;
    for (int i = 0; i < nums; ++i) {
        eh = static_cast<EventHandler *> (e_events_[i].data.ptr);
        eh->set_revents(e_events_[i].events);
        eh->HandleEvent();
    }
}

void Selector::UpdataOneEventHandler(int option, EventHandler * ehandler) {
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
        LOG << "Selector::UpdataOneEventHandler() : error option" ;
        break;
    }
}

void Selector::AddEvent(EventHandler * eh) {
    struct epoll_event e_event;
    bzero(&e_event, sizeof(struct ::epoll_event));
    int fd = eh->fd();
    DLOG << "Selector::AddEvent : fd : " << fd;
    e_event.data.fd = fd;
    e_event.data.ptr = static_cast<void *> (eh);
    e_event.events = eh->events();
    int ret = epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &e_event);  // 也许可以把普通的 read update 直接合并到一起
    if (ret < 0) {
        LOG << "Selector::AddEvent : error in add " << ret;
    }
}

void Selector::DeleteEvent(EventHandler * eh) {
    struct epoll_event e_event;
    bzero(&e_event, sizeof(struct ::epoll_event));
    int fd = eh->fd();
    DLOG << "Selector::DeleteEvent : fd : " << fd;
    e_event.data.fd = fd;
    e_event.data.ptr = static_cast<void *> (eh);
    e_event.events = eh->events();
    int ret = epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &e_event);
    if (ret < 0) {
        LOG << "Selector::DeleteEvent : error in delete " << ret;
    }
}

void Selector::ModifyEvent(EventHandler * eh) {
    struct epoll_event e_event;
    bzero(&e_event, sizeof(struct ::epoll_event));
    int fd = eh->fd();
    DLOG << "Selector::ModifyEvent : fd : " << fd;
    e_event.data.fd = fd;
    e_event.data.ptr = static_cast<void *> (eh);
    e_event.events = eh->events();
    int ret = epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &e_event);
    if (ret < 0) {
        LOG << "Selector::ModifyEvent : error in mod" << ret;
    }
}

}  // namespace yxalp
