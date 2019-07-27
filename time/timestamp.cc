#include "timestamp.h"

#include <sys/time.h>
#include <stdio.h>

using namespace yxalp;

Timestamp::Timestamp()
    : micro_seconds_since_epoch_(0) {
}

Timestamp::Timestamp(int64_t microseconds)
    : micro_seconds_since_epoch_(microseconds) {
}

std::string Timestamp::ToString() const {
    char buf[32] {};
    int64_t seconds = micro_seconds_since_epoch_ / kMicroSecondsPerSecond;
    int64_t microseconds_remainder = 
        micro_seconds_since_epoch_ % kMicroSecondsPerSecond;
    snprintf(buf, 32, "%ld.%06ld", seconds, microseconds_remainder);
    return std::string(buf);
}

std::string Timestamp::ToFormattedString() const {
    char buf[32] {};
    time_t seconds = static_cast<time_t> (micro_seconds_since_epoch_ / kMicroSecondsPerSecond);
    int microseconds_remainder = 
        micro_seconds_since_epoch_ % kMicroSecondsPerSecond;
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
        microseconds_remainder);
    return buf;
}

Timestamp Timestamp::now() {
    struct timeval tv;
    // struct timezone
    gettimeofday(&tv, nullptr);
    return Timestamp(tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec);
}

Timestamp Timestamp::invalid() {
    return Timestamp();
}