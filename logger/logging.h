// yx
// TODO 设置 LOG level
#ifndef YXALPHA_LOGGING_H_
#define YXALPHA_LOGGING_H_

#include <string>
#include <iostream>  // debug

#include "logstream.h"

// disable 这 3 个宏的状态下, async log 线程不会静默启动
// #define ENABLE_LOG
// #define DEBUGLOG
// #define STDLOG

namespace yxalp {

class Logger {
public:
    Logger(const char *file_name, int line);
    ~Logger();

    Logger(const Logger &) = delete;
    Logger operator=(const Logger &) = delete;

    LogStream& Stream() { return impl_.stream_; }

    static void set_log_file_name(std::string file_name) {
        log_file_name_ = file_name;
    }
    static std::string get_log_file_name() {
        return log_file_name_;
    }

private:
    class Impl {
    public:
        Impl(const char *file_name, int line);
        void FormatTime();

        LogStream stream_;
        int line_;
        std::string base_name_;
    };
    Impl impl_;
    static std::string log_file_name_;
};

// 什么都不做. TODO need better design
class LogNothing {
public:
    LogNothing & operator<<(bool value) { return *this; }
    LogNothing & operator<<(short int value) { return *this; }
    LogNothing & operator<<(unsigned short value) { return *this; }
    LogNothing & operator<<(int value) { return *this; }
    LogNothing & operator<<(unsigned value) { return *this; }
    LogNothing & operator<<(long int value) { return *this; }
    LogNothing & operator<<(unsigned long value) { return *this; }
    LogNothing & operator<<(long long value) { return *this; }
    LogNothing & operator<<(unsigned long long value) { return *this; }
    LogNothing & operator<<(float value) { return *this; }
    LogNothing & operator<<(double value) { return *this; }
    LogNothing & operator<<(long double value) { return *this; }
    LogNothing & operator<<(const void * value) { return *this; }
    LogNothing & operator<<(const char * value) { return *this; }
    LogNothing & operator<<(const unsigned char * value) { return *this; }
};  // class LogNothing

extern LogNothing log_nothing;

}  // namespace yxalp

// TODO log 设置不同后端(better way)
#ifdef ENABLE_LOG
    #define LOG yxalp::Logger(__FILE__, __LINE__).Stream()
    #ifdef DEBUGLOG
        #define DLOG yxalp::Logger(__FILE__, __LINE__).Stream()
    #else
        #define DLOG yxalp::log_nothing
    #endif
#else
    #define DLOG yxalp::log_nothing
    #define LOG yxalp::log_nothing
#endif

// #define LOG if(LOG_ENABLE) 
// yxalp::Logger(__FILE__, __LINE__).Stream()

#endif  // YXALPHA_LOGGING_H_
