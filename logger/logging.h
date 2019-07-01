// yx
// TODO 设置 LOG level
#ifndef YXALPHA_LOGGING_H_
#define YXALPHA_LOGGING_H_

#include <string>

#include "logstream.h"
#define ENABLE_LOG

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

}  // namespace yxalp

#define LOG yxalp::Logger(__FILE__, __LINE__).Stream()


// #define LOG if(LOG_ENABLE)
// yxalp::Logger(__FILE__, __LINE__).Stream()


#endif  // YXALPHA_LOGGING_H_
