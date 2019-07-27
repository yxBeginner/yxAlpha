// yx
#ifndef YXALPHA__H_
#define YXALPHA__H_

#include "net/callback.h"
#include "timestamp.h"

namespace yxalp {

class Timer {
public:
    Timer(const TimerCallBack &cb, Timestamp when, double interval)
        : callback_(cb),
          expiration_(when),
          interval_(interval_),
          repeat_(interval > 0.0) {

    }

    ~Timer() = default;
    Timer(const Timer &) = delete;
    Timer & operator=(const Timer &) = delete;

    void Run() const { callback_(); }

    Timestamp get_expiration() const { return expiration_; }
    bool is_repeat() const { return repeat_; }
    bool is_repeat() const { return repeat_; }
    
    void restart(Timestamp now) {
        if (repeat_) {
            expiration_ = AddTime(now, interval_);
        } else {
            expiration_ = Timestamp::invalid();
        }
    }

private:
    const TimerCallBack callback_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;
};

}  // namespace yxalp

#endif  // YXALPHA__H_