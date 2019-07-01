// Auther: yx
# ifndef YXALPHA_THREAD_H_
# define YXALPHA_THREAD_H_

#include <pthread.h>
#include <functional>

namespace yxalp {

typedef pthread_t tid_t;  // pid_t

class Thread {
public:
    typedef std::function<void ()> ThreadFunc;

    explicit Thread(const ThreadFunc &func, const std::string name = std::string());
    // 禁用
    Thread(const Thread& ) = delete;
    Thread& operator=(const Thread& ) = delete;
    ~Thread();

    void Start();
    void Join();
    bool started() const { return started_; }
    const std::string & name() const { return name_; }

private:
    bool started_;
    bool joined_;
    pthread_t thread_id_;
    tid_t tid_ ;  // TODO 实际的线程标识符, muduo recommend
    // pthread_attr_t thread_attr_;
    std::string name_;
    ThreadFunc func_;
};

// 有关当前线程的信息
namespace CureentThread {
    // 返回线程标识, 不要在 main thread 中调用
    tid_t tid();
    const char * name();
}  // CureentThread

}  // namespace yxalp
#endif  // YXALPHA_THREAD_H_