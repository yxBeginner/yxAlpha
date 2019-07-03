// yx
#ifndef YXALPHA_THREADPOOL_H_
#define YXALPHA_THREADPOOL_H_

#include <deque>
#include <vector>
#include <memory>

#include "thread.h"
#include "mutex.h"
#include "condition.h"

namespace yxalp {

class ThreadPool {
public:
    typedef std::function<void()> Task;

    ThreadPool(size_t max_queue_size, const std::string& name = std::string("ThreadPool"));
    ~ThreadPool();

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool & operator=(const ThreadPool &) = delete;

    void Start(int num_threads);
    void Stop();
    // 如果池子是空的, 则生产者会自行执行运算任务
    // 注意任务应当是非阻塞的
    void PushTask(Task task);

private:
    ThreadPool::Task PopTask();
    void RunTask();

    size_t max_queue_size_;
    std::string name_;
    bool running_;
    mutable MutexLock mutex_;
    Condition cond_full_;
    Condition cond_empty_;
    std::vector<std::unique_ptr<Thread> > worker_threads_;
    std::deque<Task> queue_;
};

}  // namespace yxalp

#endif  // YXALPHA_THREADPOOL_H_