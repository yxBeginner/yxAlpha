// yx
#ifndef YXALPHA_DISPATCHTHREAD_H_
#define YXALPHA_DISPATCHTHREAD_H_

#include "thread/mutex.h"
#include "thread/condition.h"
#include "thread/thread.h"

namespace yxalp {

class Dispatcher;

// 注意通常本类被主线程调用, 但内部的 Dispatcher 需要在 IO 线程中构造.
class DispatchThread {
public:
    DispatchThread();
    ~DispatchThread();
    DispatchThread& operator=(const DispatchThread &) = delete;
    DispatchThread(const DispatchThread &) = delete;

    Dispatcher * Start();
    // Dispatcher * get_dispatcher();  // 单独的存取函数也需要同步处理    
private:
    void ThreadFunc();
    Dispatcher * dispatcher_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    
};

}

#endif  // YXALPHA_DISPATCHTHREAD_H_