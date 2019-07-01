// yx
#include "fileutil.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

namespace yxalp {

AppendFile::AppendFile(std::string file_name)
    : fp_(fopen(file_name.c_str(), "ae")) {
    setbuffer(fp_, buffer_, sizeof buffer_);  // 设置标准 IO 的 buffer
}

AppendFile::~AppendFile() {
    fclose(fp_);
}

// 反复写, 直到写够 len (阻塞)
void AppendFile::Append(const char* logline, const size_t len) {
    size_t n = fwrite_unlocked(logline, 1, len, fp_);
    size_t remain = len - n;
    while (remain > 0) {
        size_t x = fwrite_unlocked(logline + n, 1, remain, fp_);
        if (x == 0) {
            int err = ferror(fp_);
            if (err)
                fprintf(stderr, "AppendFile::append() failed !\n");
            break;
        }
        n += x;
        remain = len - n;
    }
}

void AppendFile::Flush() {
    fflush(fp_);
}

}  // namespace yxalp
