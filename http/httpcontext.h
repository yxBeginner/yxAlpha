// yx
#ifndef YXALPHA_HTTPCONTEXT_H_
#define YXALPHA_HTTPCONTEXT_H_

#include "httprequest.h"

namespace yxalp {

class Buffer;

class HttpContext {
public:
    enum HttpRequestParseState {
        ExpectRequestLine,
        ExpectHeaders,
        ExpectBody,
        Done
    };

    HttpContext() 
        : state_(ExpectRequestLine) {

    }

    bool ParseRequest(Buffer* buf);

    bool is_done() { return state_ == Done; }

    void Reset() {
        state_ = ExpectRequestLine;
        HttpRequest dummy;
        // todo
    }

    const HttpRequest& get_request() const { return request_; }
    HttpRequest& get_request() { return request_; }

private:
    // 处理请求行
    bool ParseRequestLine(const char* begin, const char* end);

    HttpRequestParseState state_;
    HttpRequest request_;
};

}  // namespace yxalp

#endif  // YXALPHA_HTTPCONTEXT_H_