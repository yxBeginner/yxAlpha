// yx
#include "threadpool.h"

#include <cassert>
// #include "logger/logging.h"

namespace yxalp {

// 时刻注意 queue 是跨线程的

ThreadPool::ThreadPool(size_t max_queue_size, const std::string& name)
    : max_queue_size_(max_queue_size),
      name_(name),
      running_(false),
      mutex_(),
      cond_full_(mutex_),
      cond_empty_(mutex_),
      worker_threads_(),
      queue_() {

}

ThreadPool:: ~ThreadPool() {
    if (running_) {
        Stop();
    }
}

// stop 之后可以 start 新一轮.
void ThreadPool::Start(int num_threads) {
    assert(!running_);
    running_ = true;
    worker_threads_.resize(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        std::string thread_name = name_ + std::to_string(i+1);
        worker_threads_[i] = std::make_unique<Thread> (
            std::bind(&ThreadPool::RunTask, this), thread_name);
        worker_threads_[i]->Start();
        // LOG >> "name_ start";
    }
}

 // 启动前调用或被多次调用无害
void ThreadPool::Stop() {
    {
        MutexLockGuard lock_up(mutex_);
        running_ = false;
        cond_empty_.notifyAll();
    }
    for (auto &thread: worker_threads_) {
        thread->Join();
    }
}

void ThreadPool::PushTask(Task task) {
    if (worker_threads_.empty()) {
        task();
    } else {
        MutexLockGuard lock_up(mutex_);
        while (max_queue_size_ >0 && queue_.size() >= max_queue_size_) {
            cond_full_.wait();
        }
        assert(queue_.size() <= max_queue_size_);
        queue_.push_back(std::move(task));
        cond_empty_.notify();
    }
}

ThreadPool::Task ThreadPool::PopTask() {
    MutexLockGuard lock_up(mutex_);
    while (queue_.empty() && running_) {
        cond_empty_.wait();  // 两种唤醒方式, 需要区别对待
    }

    Task task;
    if (!queue_.empty()) {
        task = queue_.front();
        queue_.pop_front();
        assert(queue_.size() < max_queue_size_);
        if (max_queue_size_ > 0) {  // or assert?
            cond_full_.notify();
        }
    }
    return std::move(task);
}

// 每个线程池的线程执行无限循环, 从任务队列中取出任务执行
void ThreadPool::RunTask() {
    // TODO try catch
    while (running_) {
        // Task p1 = nullptr;
        Task task(PopTask());  // function 对象
        if (task) {  // 有可能传递过来空任务
            task();
        }
    }
}

}  // namespace yxalp