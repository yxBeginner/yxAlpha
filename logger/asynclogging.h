// yx
#ifndef YXALPHA_ASYNCLOGGING_H_
#define YXALPHA_ASYNCLOGGING_H_

#include <vector>
#include <memory>
// #include <atomic>

#include "thread/thread.h"
#include "thread/countdownlatch.h"
#include "logstream.h"

namespace yxalp {

class AsyncLogging {
public:
    AsyncLogging(std::string file_name, int flushInterval);
    
    ~AsyncLogging() {
        if (running_) {
            Stop();
        }
    }

    AsyncLogging(const AsyncLogging &) = delete;
    AsyncLogging operator=(const AsyncLogging &) =delete;

    // 前端写, thread safe
    void Append(const char* logline, size_t len);

    void Start() {
        running_ = true;
        thread_.Start();
        latch_.Wait();
    }

    void Stop() {
        running_ = false;
        cond_.notify();  // 刷出
        thread_.Join();
    }

private:
    void ThreadFunc();
    
    typedef FixedBuffer<kBigSize> Buffer;
    typedef std::shared_ptr<Buffer> BufferPtr;
    typedef std::vector<BufferPtr > BufferVector;
    std::string file_name_;
    const int flushInterval_;  // 刷出间隔
    Thread thread_;
    bool running_;
    CountDownLatch latch_;  // 
    MutexLock mutex_;
    Condition cond_;
    BufferPtr current_buffer_;
    BufferPtr next_buffer_;
    BufferVector buffers_;
};

}  // namespace yxalp

#endif  // YXALPHA_ASYNCLOGGING_H_