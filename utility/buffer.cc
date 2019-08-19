#include "buffer.h"

#include <sys/uio.h>

namespace yxalp {

const char * Buffer::CRLF = "\r\n";

Buffer::Buffer()
    : buffer_(kBufBegin + kDefaultSize), 
      read_index_(kBufBegin),
      write_index_(kBufBegin) {
          
}

ssize_t Buffer::readFd(int fd, int* saved_errno) {
    char extrabuf[4096];
    struct iovec vec[2];  // 两块 buffer
    const size_t writable = remain_size();
    vec[0].iov_base = raw_begin()+write_index_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;  // 指向栈上 buffer 的开始地址
    vec[1].iov_len = sizeof (extrabuf);
    const int iov_cnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = readv(fd, vec, iov_cnt);  // read or write data into multiple buffers
    if (n < 0) {
        *saved_errno = errno;
    } else if (static_cast<size_t>(n) <= writable) {
        write_index_ += n;
    } else {
        write_index_ = buffer_.size();
        Append(extrabuf, n - writable);  // 栈上数据
    }
    return n;
}

void Buffer::Resize(size_t len) {
    // buffer 内部空间不足
    if ((read_index_ - kBufBegin + remain_size() < len)) {
        buffer_.resize(write_index_ + len);  // no extra space
    } else {
        // 内部移动
        size_t p_size = payload_size();
        std::copy(raw_begin() + read_index_, raw_begin() + write_index_, 
                            raw_begin() + kBufBegin);
        read_index_ = kBufBegin;
        write_index_ = read_index_ + p_size;
    }
}

};  // namespace yxalp