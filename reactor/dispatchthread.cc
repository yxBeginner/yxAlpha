// yx
#include "dispatchthread.h" 

#include <functional>
#include <string>
#include "dispatcher.h"

namespace yxalp {

DispatchThread::DispatchThread()
    : dispatcher_(nullptr),
      thread_(std::bind(&DispatchThread:: ThreadFunc, this), 
                        std::string("IO Thread ") + 
                        std::to_string(reinterpret_cast<unsigned long> (this))),  // ugly
      mutex_(),
      cond_(mutex_) {

      }

DispatchThread::~DispatchThread() {
    dispatcher_->Quit();
    thread_.Join();
}

Dispatcher * DispatchThread::Start() {
    assert(!thread_.started());
    thread_.Start();
    MutexLockGuard lock_up(mutex_);
    while (dispatcher_ == nullptr) {
        cond_.wait();
    }
    return dispatcher_;
}

void DispatchThread::ThreadFunc() {
    // dispatcher 与 IO thread 是一对一关系, 由 IO 线程在其内部构造
    Dispatcher dispatcher;
    {  // 临界区
    MutexLockGuard lock_up(mutex_);
    dispatcher_ = &dispatcher;
    cond_.notify();
    }  // 临界区
    dispatcher_->Loop();  // Loop 之前释放锁
}

}  // namespace yxalp
