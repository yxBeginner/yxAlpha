// yx
#ifndef YXALPHA__H_
#define YXALPHA__H_

#include <vector>
#include <memory>

namespace yxalp {

class Dispatcher;
class DispatchThread;

class DispatcherPool {
public:
    DispatcherPool(Dispatcher *base);
    ~DispatcherPool();
    DispatcherPool(const Dispatcher &) = delete;
    DispatcherPool& operator=(const Dispatcher &) = delete;

    void set_thread_num(int num) { num_threads_ = num; };
    void Start();
    Dispatcher * get_next_dispatcher();

private:
    Dispatcher * base_;
    bool  started_;
    int num_threads_;
    int next_;
    std::vector<std::unique_ptr<DispatchThread>> threads_;
    std::vector<Dispatcher *> dispatchers_;
};

}  // namespace yxalp

#endif  // YXALPHA__H_