#ifndef YXALPHA_TIMESTAMP_H_
#define YXALPHA_TIMESTAMP_H_

#include <stdint.h>
#include <string>

namespace yxalp {

// 时间戳 Time stamp in UTC, after Epoch
class Timestamp {
public:
    Timestamp();

    explicit Timestamp(int64_t micro_seconds_since_epoch);

    void Swap(Timestamp &rhs) {  // unsafe
        int64_t tmp = rhs.micro_seconds_since_epoch_;
        rhs.micro_seconds_since_epoch_ = this->micro_seconds_since_epoch_;
        this->micro_seconds_since_epoch_ = tmp;
    }

    std::string ToString() const;
    std::string ToFormattedString() const;

    bool is_valid() const { return micro_seconds_since_epoch_ > 0; }

    int64_t micro_seconds_since_epoch() const { return micro_seconds_since_epoch_; }

    static Timestamp now();

    static Timestamp invalid();

    bool operator<(const Timestamp &rhs) 
    { return this->micro_seconds_since_epoch_ <  rhs.micro_seconds_since_epoch_;}

    bool operator==(const Timestamp &rhs) 
    { return this->micro_seconds_since_epoch_ ==  rhs.micro_seconds_since_epoch_;}

    static const int kMicroSecondsPerSecond = 1000 * 1000;  // 秒与微秒的倍数
private:
    friend Timestamp AddTime(Timestamp ts, double seconds);
    int64_t micro_seconds_since_epoch_;
};

inline Timestamp AddTime(Timestamp ts, double seconds) {
    int64_t t = static_cast<int64_t> (seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(ts.micro_seconds_since_epoch_ + t);
}

}  // namespace yxalp

#endif  // YXALPHA_TIMESTAMP_H_