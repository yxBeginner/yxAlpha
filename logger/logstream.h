// yx
#ifndef YXALPHA_LOGSTREAM_H_
#define YXALPHA_LOGSTREAM_H_

#include <string.h>

namespace yxalp {

const int kDefaultSize = 4000;
const int kBigSize = 4000 * 1000;

template<int N> class FixedBuffer {
public:
    FixedBuffer() : cur_(data_) {}
    ~FixedBuffer() {}

    FixedBuffer(const FixedBuffer &) = delete;
    FixedBuffer& operator=(const FixedBuffer &) = delete;

    // 不在 buffer 中处理数据量问题
    void Append(const char * buf, size_t len) {
        if (available() > len) {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }
    
    const char * get_data() const { return data_; }
    char * current() { return cur_; }
    size_t current_length() const { return cur_ - data_; }
    size_t available() const { return (N - (cur_ - data_)); }
    void add(size_t len) { cur_ += len; }
    void reset() { cur_ = data_; }
    void ClearBuffer() { bzero(data_, N); }

private:
    char data_[N];
    char * cur_;
};


class LogStream {
public:
    typedef FixedBuffer<kDefaultSize> Buffer;

    LogStream() {}
    ~LogStream() {}
    LogStream(const LogStream &) = delete;
    LogStream& operator=(const LogStream &) = delete;

    // 不是一般使用的是 int64_t 这种吗?
    LogStream& operator<<(bool value);
    LogStream& operator<<(short int value);
    LogStream& operator<<(unsigned short value);
    LogStream& operator<<(int value);
    LogStream& operator<<(unsigned value);
    LogStream& operator<<(long int value);
    LogStream& operator<<(unsigned long value);
    LogStream& operator<<(long long value);
    LogStream& operator<<(unsigned long long value);
    LogStream& operator<<(float value);
    LogStream& operator<<(double value);
    // LogStream& operator<<(long double value);
    // LogStream& operator<<(const void * value);
    LogStream& operator<<(const char * value);
    LogStream& operator<<(const unsigned char * value);

    void Append(const char * buf, size_t len) { buffer_.Append(buf, len); }
    // 暴露内部 buffer
    const Buffer & get_buffer() const { return buffer_; }
    void reset_buffer() { buffer_.reset(); }

private:
    static const int kMaxNumericSize = 32;
    template<typename T> void FormatInteger(T v);  // gcc
    Buffer buffer_;  // 行缓冲
};

}  // namespace yxalp

#endif  // YXALPHA_LOGSTREAM_H_