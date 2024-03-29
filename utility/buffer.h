// yx
#ifndef YXALPHA_BUFFER_H_
#define YXALPHA_BUFFER_H_

#include <vector>
#include <cstring>
#include <algorithm>
#include <string>
#include <cassert>

namespace yxalp {

// 不要使用 vector<char> 自身的 end(), size(), 不表示实际数据的位置
// |savedSpace|---------------|payload-------|-------------------end|
// |----------------kBufBegin--read_index_--write_index_--buffer_.size()
class Buffer {
public:
    static const size_t kBufBegin = 8;
    static const size_t kDefaultSize = 256;
    
    Buffer();
    ~Buffer() = default;
    Buffer(const Buffer&) = default;
    Buffer& operator=(const Buffer&) = default;

    size_t payload_size() const {
        return write_index_ - read_index_;
    }

    size_t remain_size() const {
        return buffer_.size() - write_index_;
    }

    const char * peek() const {
        return raw_begin() + read_index_;
    }

    // 需要客端手动释放
    char * GetCharStr(size_t len) {
        size_t min_len = (len > payload_size()) ? payload_size() : len;
        read_index_ += min_len;
        char * data = new char[len];
        memcpy(data, peek(), len);
        return data;
    }

    // 与 peek() 一起使用
    void move_read_index(size_t len) {
        assert(len <= payload_size());
        read_index_ += len;
    }

    void move_read_index_until(const char *end) {
        assert(end <= begin_write());
        assert(peek() <= end);
        move_read_index(end - peek());
    }

    // 与 peek() 一起使用
    void move_read_index_all() {
        read_index_ = write_index_ = kBufBegin;  // 归档
    }

    // 取出 buf 中的 payload, read_index 会移动
    std::string GetString() {
        std::string str(peek(), payload_size());
        move_read_index_all();
        return str;
    }

    // 取出 buf 中的 payload, read_index 不会移动
    std::string GetStringNoMove() {
        std::string str(peek(), payload_size());
        return str;
    }

    void Append(const char* data, size_t len) {
        if (remain_size() < len) {
            Resize(len);
        }
        std::copy(data, data+len, raw_begin() + write_index_);
        write_index_ += len;
    }

    void Append(const std::string &str) {
        Append(str.c_str(), str.size());
    }

    void Append(const void* data, size_t len) {
        Append(static_cast<const char*> (data), len);
    }
    // 从内核中读数据
    ssize_t readFd(int fd, int* saved_errno);

    const char * findCRLF() const {
        const char * crlf = std::search(peek(), begin_write(), CRLF, CRLF+2);
        return crlf == begin_write() ? nullptr : crlf;
    }

    const char * findCRLF(const char * start) const {
        assert(peek() <= start);
        assert(start <= begin_write());
        const char * crlf = std::search(start, begin_write(), CRLF, CRLF+2);
        return crlf == begin_write() ? nullptr : crlf;
    }

private:
    // 返回 buffer 中第一个字符的地址, 以使用 memmove()
    char * raw_begin() {
        return &(buffer_.front());
    }

    const char * raw_begin() const {
        return &(buffer_.front());
    }

    const char * begin_write() const {
        return raw_begin() + write_index_;
    }

    char * begin_write() {
        return raw_begin() + write_index_;
    }

    void Resize(size_t len);

    std::vector<char> buffer_;
    size_t read_index_;  // for both side
    size_t write_index_;
    static const char *CRLF;
};

}  // namespace yxalp

#endif  // YXALPHA_BUFFER_H_