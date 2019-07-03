// yx
#ifndef YXALPHA_FILEUTIL_H_
#define YXALPHA_FILEUTIL_H_

#include "string"

namespace yxalp {

class AppendFile {
public:
    explicit AppendFile(std::string file_name);
    ~AppendFile();

    AppendFile(const AppendFile &) = delete;
    AppendFile& operator=(const AppendFile &) = delete;

    void Append(const char *logline, const size_t len);
    void Flush();

private:
    FILE *fp_;
    char buffer_[64*1024];
};

}  // namespace yxalp

#endif  // YXALPHA_FILEUTIL_H_