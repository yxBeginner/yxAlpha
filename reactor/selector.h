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

class EventHandler;
class Timestamp;

class Selector {
public:
    enum { ADD = 0, DEL, MOD };
    Selector(); 
    ~Selector();
    Selector & operator=(const Selector &) = delete;
    Selector(const Selector &) = delete;

    // Timestamp Select(int timeout);
    void Select(int timeout);

    // 可用 ADD, DEL, MOD, 但是不可以使用并集 
    void UpdataOneEventHandler(int option, EventHandler * ehandler);
    
private:
    void UpdataTriggeredEventHandlers(int);
    void AddEvent(EventHandler * eh);
    void DeleteEvent(EventHandler * eh);
    void ModifyEvent( EventHandler * eh);

    static const int kEEventsSize = 128;

    int epollfd_;
    vector<epoll_event> e_events_;
};

}  //namespace yxalp

#endif  // YXALPHA_SELECTOR_H_