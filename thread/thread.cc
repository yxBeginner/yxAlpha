// yx
// TODO 暂时不像 muduo 的做法, 只用了 pthread_t 做线程标识符
#include "thread.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <cassert>

namespace yxalp {

namespace CureentThread {

__thread tid_t t_tid = 0;  //  线程标识
__thread const char* t_name = "unknown";  // 线程名称

tid_t tid() {
    return t_tid;
}

const char * name() {
    return t_name;
}

}  // namespace CureentThread

// 线程属性结构体, 在线程启动时, 设置本线程相应的属性
struct ThreadData {
    Thread::ThreadFunc func_;
    std::string name_;
    tid_t* tid_;  // 引用

    ThreadData(const Thread::ThreadFunc func, const std::string &name, tid_t *tid)
        : func_(func),
          name_(name),
          tid_(tid) {
        
    }

    void RunInThread() {
        // CureentThread::t_tid = *tid_;  // TODO 不妥
        CureentThread::t_tid = (static_cast<tid_t> (pthread_self()));
        CureentThread::t_name = name_.c_str();
        func_();
    }
};  // struct ThreadData

// 线程入口函数, 执行客户定义的 function
void * StartRoutine(void * arg) {
    ThreadData *data = static_cast<ThreadData *> (arg);
    data->RunInThread();
    delete data;
    return NULL;  // exit
}

Thread::Thread(const ThreadFunc &func, const std::string name) 
    : started_(false),
      joined_(false),
      thread_id_(0),
      tid_(static_cast<tid_t> (thread_id_)),  // TODO 直接使用pthread_t 设置了.
      name_(name),
      func_(func) {
          if (name_.empty()) name_.assign("default");
}

// 析构函数没有回收 pthread_t, 暂不考虑运行中线程的退出
Thread::~Thread() {
    // pthread_exit(NULL);
    if (started_ && !joined_) {
        pthread_detach(thread_id_);
    }
}

void Thread::Start() {
    assert(!started_);
    started_ = true;
    // 此时线程还未启动, thread_t 无效, 直接传递 pthread_t ?
    ThreadData * data = new ThreadData(func_, name_, &tid_);
    int ret = pthread_create(&thread_id_, NULL, &StartRoutine, data);
    if (ret != 0) {
        started_ = false;
        delete data;
        exit(ret);
    } else {
        // direct log
    }
}

void Thread::Join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    pthread_join(thread_id_, NULL);  // 创建者对本线程对象内的线程 join
}

}  // namespace yxalp