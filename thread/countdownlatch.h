#ifndef YXALPHA_COUNTDOWNLATCH_H_
#define YXALPHA_COUNTDOWNLATCH_H_

#include "mutex.h"
#include "condition.h"

namespace yxalp {

class CountDownLatch {
 public:
    explicit CountDownLatch(int count)
        : mutex_(),
          condition_(mutex_),
          count_(count) {
    }
    
    ~CountDownLatch() = default;
    CountDownLatch(const CountDownLatch &) = delete;
    CountDownLatch& operator=(const CountDownLatch &) = delete;

    void Wait() {
        MutexLockGuard lock_up(mutex_);
        while (count_ > 0) {
            condition_.wait();
        }
    }

    void CountDown() {
        MutexLockGuard lock_up(mutex_);
        --count_;
        if (count_ == 0) {
            condition_.notifyAll();
        }
    }

    int GetCount() const {
        MutexLockGuard lock_up(mutex_);
        return count_;
    }

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};

}  // namespace yxalp
#endif  // YXALPHA_COUNTDOWNLATCH_H_
