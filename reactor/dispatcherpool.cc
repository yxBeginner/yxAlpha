//yx

#include "dispatcherpool.h"
#include "dispatchthread.h"
#include "dispatcher.h"
#include "logger/logging.h"

namespace yxalp {

DispatcherPool::DispatcherPool(Dispatcher *base) 
    : base_(base),
      started_(false),
      num_threads_(0),
      next_(0) {
    
}

DispatcherPool::~DispatcherPool() {
    // SubDispatcher 是 stack 对象
    // Dispatcher * release by DispatchThread
}

void DispatcherPool::Start() {
    base_->AssertInLoopThread();
    started_ = true;
    for (int i=0; i < num_threads_; ++i) {  
        // threads_.push_back(std::make_unique<DispatchThread> ());
        // dispatchers_.push_back(threads_.back().get()->Start());
        DispatchThread *t = new DispatchThread;
        threads_.push_back(std::unique_ptr<DispatchThread>(t));
        dispatchers_.push_back(t->Start());
    }
    DLOG << "DispatcherPool::Start() : nums of threads : " << dispatchers_.size();
}

Dispatcher * DispatcherPool::get_next_dispatcher() {
    base_->AssertInLoopThread();
    Dispatcher * dispatcher = base_;  // in case dispatchers_ is empty
    if (!dispatchers_.empty()) {
        // round-robin
        DLOG << "DispatcherPool::get_next_dispatcher() : SubDIspatcher Id : " 
                     << next_;
        dispatcher = dispatchers_[next_];
        next_ = (next_ + 1) % num_threads_;
    }
    return dispatcher;
}

}  // namespace yxalp
