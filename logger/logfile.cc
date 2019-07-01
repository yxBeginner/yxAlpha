// yx
#include "logfile.h"

#include "thread/mutex.h"
#include "fileutil.h"

namespace yxalp {

LogFile::LogFile(const std::string &base_file_name, bool thread_safe, 
                                  int flush_interval, int check_every_n)
    : base_file_name_(base_file_name),
      flush_interval_(flush_interval),
      check_every_n_(check_every_n),
      mutex_((thread_safe ? std::make_unique<MutexLock> () : nullptr)),
      file_(std::make_unique<AppendFile> (base_file_name)) {

}

void LogFile::Append(const char * logline, int len) {
    if (mutex_) {
        MutexLockGuard lock_up(*mutex_);
        Append_(logline, len);
    } else {
        Append_(logline, len);
    }
}

void LogFile::Flush() {
    if (mutex_) {
        MutexLockGuard lock_up(*mutex_);
        file_->Flush();
    } else {
        file_->Flush();
    }
}

LogFile::~LogFile() {
}

void LogFile::Append_(const char * logline, int len) {
    file_->Append(logline, len);
    ++count_;
    if (count_ >= check_every_n_) {
        count_ = 0;
        file_->Flush();
    }
}

}  // namespace yxalp