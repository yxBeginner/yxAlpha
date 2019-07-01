// yx
// Synchronous Event Demultiplexer
#ifndef YXALPHA_SELECTOR_H_
#define YXALPHA_SELECTOR_H_

#include <vector>
#include <map>

#include "eventhandler.h"

struct epoll_event;

namespace yxalp {

using std::vector;
using std::map;

class EventHandler;

// Selector 缓存 pollfds, 与 Dispatcher 一对一
class Selector {
public:
    enum { ADD = 0, DEL, MOD };
    // Selector() = default;
    // ~Selector()  = default;
    Selector(); 
    ~Selector();
    Selector & operator=(const Selector &) = delete;
    Selector(const Selector &) = delete;

    void Select(int timeout, vector<EventHandler*> *handlers);

    // 可用 ADD, DEL, MOD, 但是不可以使用并集 
    void UpdataOneEventHandler(int option, EventHandler * ehandler);
    
private:
    void UpdataTriggeredEventHandlers(int , vector<EventHandler*> *);
    void AddEvent(EventHandler * eh);
    void DeleteEvent(EventHandler * eh);
    void ModifyEvent( EventHandler * eh);

    static const int kEEventsSize = 16;

    int epollfd_;
    vector<epoll_event> e_events_;
};

}  //namespace yxalp

#endif  // YXALPHA_SELECTOR_H_