#ifndef YXALPHA_MUTEX_H_
#define YXALPHA_MUTEX_H_

// Q: 应当先销毁 mutex, 还是先销毁 attribute ? 其它的也类似 ?

#include <assert.h>
#include <pthread.h>
// #include <

namespace yxalp {

class MutexLockGuard;
class Condition;

// 只允许 RAII 方式使用. lock 与 unlock 被设置为私有成员方法.
class MutexLock {
public:
    MutexLock & operator= (const MutexLock &) = delete;
    MutexLock(const MutexLock &) = delete;
    
    MutexLock() {
        pthread_mutexattr_init(&attr_);
        pthread_mutex_init(&mutex_, &attr_);
    }

    ~MutexLock() {
        pthread_mutex_destroy(&mutex_);
        pthread_mutexattr_destroy(&attr_);
    }

private:
    void lock() {
        pthread_mutex_lock(&mutex_);
    }

    void unlock() {
        pthread_mutex_unlock(&mutex_);
    }

    // 只应被 Condition 使用
    pthread_mutex_t * getPthreadMutex() {
        return &mutex_;
    }

    friend class MutexLockGuard;
    friend class Condition;

    pthread_mutex_t mutex_;
    pthread_mutexattr_t attr_;
    // pid_t holder_;
};


class MutexLockGuard {
public:
    MutexLockGuard& operator= (const MutexLockGuard &) = delete;
    MutexLockGuard(const MutexLockGuard &) = delete;
    
    explicit MutexLockGuard(MutexLock &mutex) : mutex_(mutex) {
        mutex_.lock();
    }

    ~MutexLockGuard() {
        mutex_.unlock();
    }
private:

    MutexLock &mutex_;
};


// 正确的调用应该是 MutexLockGuard lock(x), 无变量名将导致, guard对象是一个临时对象
#define MutexLockGuard(x) static_assert(false, "Missing mutex guard var name!")
}  // namespace yxalp

#endif  // YXALPHA_MUTEX_H_
