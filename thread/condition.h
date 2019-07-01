#ifndef YXALPHA_CONDITION_H_
#define YXALPHA_CONDITION_H_

#include <time.h>

#include "mutex.h"

namespace yxalp {

class Condition {
public:
    Condition& operator=(const Condition&) = delete;
    Condition(const Condition&) = delete;

    explicit Condition(MutexLock &mutex) : mutex_(mutex) {
        pthread_condattr_init(&condattr_);
        pthread_cond_init(&cond_, &condattr_);
    }

    ~Condition() {
        pthread_cond_destroy(&cond_);
        pthread_condattr_destroy(&condattr_);
    }

    void wait() {
        pthread_cond_wait(&cond_, mutex_.getPthreadMutex());
    }

    void notify() {
        pthread_cond_signal(&cond_);
    }

    void notifyAll() {
        pthread_cond_broadcast(&cond_);
    }

    bool waitForSeconds(int seconds) {
        struct timespec abstime;  // epoch
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.getPthreadMutex(), &abstime);
    }

private:
    MutexLock &mutex_;  // mutex 要先于cond 构造
    pthread_cond_t cond_;
    pthread_condattr_t condattr_;
};

}  // namespace yxalp

#endif  // YXALPHA_CONDITION_H_