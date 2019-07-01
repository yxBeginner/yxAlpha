#ifndef YXALPHA_TIMESTAMP_H_
#define YXALPHA_TIMESTAMP_H_

#include <stdint.h>

namespace yxalp {

// 时间戳
class Timestamp {

public:
    Timestamp();

    explicit Timestamp(int64_t micro_seconds_since_epoch);

    void Swap(Timestamp &rhs) {
        // std::swap();
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;  // 秒与微秒的倍数
private:
    int64_t micro_seconds_since_epoch_;
};

}

#endif  // YXALPHA_TIMESTAMP_H_