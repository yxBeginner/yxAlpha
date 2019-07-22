// yx
#ifndef YXALPHA_HTTPRESPONSE_H_
#define YXALPHA_HTTPRESPONSE_H_

#include <map>
#include <string>

namespace yxalp {

class Buffer;

class HttpResponse {
public:
    enum HttpStatusCode {
        Unknown,
        Ok = 200,
        MovePermanently = 301,
        BadRequest = 400,
        NotFound = 404
    };

    explicit HttpResponse(bool close)
        : status_code_(Unknown),
          close_connection_(close) {

    }

    void set_status_code(HttpStatusCode code) { status_code_ = code; }

    void set_status_message(const std::string& message) { status_message_ = message; }

    void set_close_connection(bool on) { close_connection_ = on; }

    bool close_connection() const { return close_connection_; }

    void set_content_type(const std::string& content_type) 
    { add_header("Content-Type", content_type); }

    void add_header(const std::string& key, const std::string& value)
    { headers_[key] = value; }

    void set_body(const std::string& body) { body_ = body; }

    void AppendToBuffer(Buffer* output) const;
    
private:
    std::map<std::string, std::string> headers_;  // 响应头
    HttpStatusCode status_code_;  //响应码
    std::string status_message_;  // 状态信息
    bool close_connection_;  // 是否 keep-alive
    std::string body_;  // 响应报文(实体)
};

}  // namespace yxalp

#endif  // YXALPHA_HTTPRESPONSE_H_