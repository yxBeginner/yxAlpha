// yx
#ifndef YXALPHA_LOGFILE_H_
#define YXALPHA_LOGFILE_H_

#include <string>
#include <memory>

namespace yxalp {

class AppendFile;
class MutexLock;

class LogFile {
public:
    LogFile(const std::string &base_file_name, bool thread_safe = false, 
                    int flush_interval = 3, int check_every_n = 1024);
    ~LogFile();
    LogFile(const LogFile &) = delete;
    LogFile operator= (const LogFile &) = delete;

    void Append(const char * logline, int len);
    void Flush();

    // bool RollFile();  // TODO 
private:
    void Append_(const char * logline, int len);

    int count_ = 0;
    const std::string base_file_name_;
    const int flush_interval_;
    const int check_every_n_;

    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;
};




}  // namespace yxalp

#endif  // YXALPHA_LOGFILE_H_