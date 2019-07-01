// yx
// 这里与 muduo 几乎一致
#include "asynclogging.h"

#include <cassert>

#include "logfile.h"

namespace yxalp {

AsyncLogging::AsyncLogging(std::string file_name, int flushInterval)
    : file_name_(file_name),
      flushInterval_(flushInterval),
      thread_(std::bind(&AsyncLogging::ThreadFunc, this), "ALogging"),
      running_(false),
      latch_(1),
      mutex_(),
      cond_(mutex_),
      current_buffer_(std::make_shared<Buffer> ()),
      next_buffer_(std::make_shared<Buffer> ()),
      buffers_() {
    current_buffer_->ClearBuffer();
    next_buffer_->ClearBuffer();
    buffers_.reserve(8u);  // 只是 8 个指针
}

void AsyncLogging::Append(const char* logline, size_t len) {
    MutexLockGuard lock_up(mutex_);  // 前端向同一个 buffer 写
    if (current_buffer_->available() > len) {
        current_buffer_->Append(logline, len);
    } else {  // 当前 buffer 写满
        buffers_.push_back(std::move(current_buffer_));  // 放到队列中
        if (next_buffer_) {  // 有备用 buffer
            current_buffer_ = std::move(next_buffer_);
        } else {  // 分配新 buffer
            current_buffer_.reset(new Buffer);  // bad situation
        }
        current_buffer_->Append(logline, len);
        cond_.notify();  // 即刻唤醒后端线程刷出
    }
}

void AsyncLogging::ThreadFunc() {
    latch_.CountDown();
    LogFile output(file_name_);
    BufferPtr new_buffer_1 =  std::make_shared<Buffer> ();  // 两块后端 buffer
    BufferPtr new_buffer_2 =  std::make_shared<Buffer> ();
    new_buffer_1->ClearBuffer();
    new_buffer_2->ClearBuffer();
    BufferVector buffers_to_write;
    buffers_to_write.reserve(8u);

    while (running_) {
        assert(new_buffer_1 && new_buffer_1->current_length() == 0);
        assert(new_buffer_2 && new_buffer_2->current_length() == 0);
        assert(buffers_to_write.empty());

        {  // 临界区
            MutexLockGuard lock(mutex_);
            if (buffers_.empty())  {  // 只有一个 wait 者
                cond_.waitForSeconds(flushInterval_);
            }
                buffers_.push_back(std::move(current_buffer_));
                current_buffer_ = std::move(new_buffer_1);  // 移用空 buffer
                buffers_to_write.swap(buffers_);
            if (!next_buffer_) {
                next_buffer_ = std::move(new_buffer_2);
            }
        }  // 临界区

        assert(!buffers_to_write.empty());

        if (buffers_to_write.size() > 25) {
            char buf[256];
            snprintf(buf, sizeof buf, "Dropped log messages , %zd larger buffers\n",
                    buffers_to_write.size()-2);
            fputs(buf, stderr);
            output.Append(buf, static_cast<int>(strlen(buf)));
            buffers_to_write.erase(buffers_to_write.begin()+2, buffers_to_write.end());
        }

        for (const auto &buffer : buffers_to_write) {  // 刷出
            output.Append(buffer->get_data(), buffer->current_length());
        }

        if (buffers_to_write.size() > 2) {
            // drop non-bzero-ed buffers, avoid trashing
            buffers_to_write.resize(2);
        }

        if (!new_buffer_1) {
            assert(!buffers_to_write.empty());
            new_buffer_1 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            new_buffer_1->reset();
        }

        if (!new_buffer_2) {
            assert(!buffers_to_write.empty());
            new_buffer_2 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            new_buffer_2->reset();
        }

        buffers_to_write.clear();
        output.Flush();
    }
    output.Flush();
}

}  // namespace yxalp
