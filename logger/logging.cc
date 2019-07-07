// yx
#include "logging.h"

#include <pthread.h>
#include <memory>
#include <ctime>
#include <sys/time.h>

#include "asynclogging.h"

namespace yxalp {

LogNothing log_nothing;
static pthread_once_t once_ = PTHREAD_ONCE_INIT;
static AsyncLogging *async_logger_;  // Singleton

std::string Logger::log_file_name_ = "./test_log.log";
// std::string Logger::log_file_name_ = "/dev/null";

void Init_() {
    async_logger_ = new AsyncLogging(Logger::get_log_file_name(), 3);
    async_logger_->Start();
}

Logger::Impl::Impl(const char *fileName, int line)
    : stream_(),
      line_(line),
      base_name_(fileName) {
    // FormatTime();  // 向 stream 中输出时间信息.
}

// TODO 目前的实现方式, 反复陷入内核, 及其耗时
void Logger::Impl::FormatTime() {
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);  // 非常耗时
    strftime(str_t, 26, " %Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}

Logger::Logger(const char *file_name, int line)
    : impl_(file_name, line) {

}

// TODO 这样做不能编译成库, 需要动态设定
Logger::~Logger() {
#ifdef ENABLE_LOG
    impl_.stream_ << " -- " << impl_.base_name_.c_str() << ':' << impl_.line_ << '\n';
    const LogStream::Buffer &buffer(Stream().get_buffer());
    #ifdef STDLOG
        std::cout << buffer.get_data() << std::endl;
    #else
        pthread_once(&once_, Init_);
        async_logger_->Append(buffer.get_data(), buffer.current_length());
    #endif
#endif
}

}  // namespace yxalp 