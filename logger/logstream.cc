// yx
#include "logstream.h"

#include <stdio.h>
#include <algorithm>

#pragma GCC diagnostic ignored "-Wtype-limits"

namespace yxalp {

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

// atoi
template<typename T>
size_t convert(char buf[], T value) {
    T i = value;
    char* p = buf;
    do {
        int lsd = static_cast<int>(i % 10);  // 余数可正可负
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0) {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);
    return p - buf;  // 使用了 buf 的长度
}

// buffer 溢出直接截断
template<typename T> void LogStream::FormatInteger(T v) {
    if (buffer_.available() >= kMaxNumericSize) {
        size_t len = convert(buffer_.current(), v);
        buffer_.add(len);
    }
}

LogStream& LogStream::operator<<(bool value) {
    buffer_.Append( value ? "1" : "0", 1);
    return *this;
}

LogStream& LogStream::operator<<(int value) {
    FormatInteger(value);
    return *this;
}

// 既然如此直接, 何必提供呢?
LogStream& LogStream::operator<<(short int value) {
    (*this) << static_cast<int> (value);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short value) {
    (*this) << static_cast<unsigned int> (value);
    return *this;
}

LogStream& LogStream::operator<<(unsigned value) {
    FormatInteger(value);
    return *this;
 }

LogStream& LogStream::operator<<(long int value) {
    FormatInteger(value);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long value) {
    FormatInteger(value);
    return *this;
}

LogStream& LogStream::operator<<(long long value) {
    FormatInteger(value);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long value) {
    FormatInteger(value);
    return *this;
}

// buffer 溢出(超长)截断
LogStream& LogStream::operator<<(double value) {
    if (buffer_.available() >= kMaxNumericSize) {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", value);  // 丢失精度
        buffer_.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(float value) {
    *this << static_cast<double>(value);
    return *this;
}

// LogStream& LogStream::operator<<(long double value) {
//     if (buffer_.available() >= kMaxNumericSize) {
//         int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", value);  // 丢失精度
//         buffer_.add(len);
//     }
//     return *this;
// }

LogStream& LogStream::operator<<(const char * value) {
    if (value == nullptr) {
        buffer_.Append("(NULL)",  6);  // 一致
    } else {
        buffer_.Append(value,  strlen(value));        
    }
    return *this;
}

LogStream& LogStream::operator<<(const unsigned char * value) {
    return operator<<(reinterpret_cast<const char *> (value));  // static_cast is not allow
}

}  // namespace yxalp
