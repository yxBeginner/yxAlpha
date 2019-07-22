// yx
#ifndef YXALPHA_HTTPREQUEST_H_
#define YXALPHA_HTTPREQUEST_H_

#include <map>
#include <string>

namespace yxalp {

class HttpRequest {
public:
    enum Method {
        INVALID, GET, POST, HEAD, PUT, DELETE
    };
    enum Version {
        UNKNOWN, HTTP10, HTTP11
    };

    HttpRequest()
         : method_(INVALID),
           version_(UNKNOWN) {

    }

    void set_version(Version v) { version_ = v; };
    Version get_version() const { return version_;};
    
    bool set_method(const char* start, const char* end) {
        std::string method(start, end);
        if (method == "GET") {
            method_ = GET;
        } else if (method == "POST") {
            method_ = POST;
        } else if (method == "HEAD") {
            method_ = HEAD;
        } else if (method == "PUT") {
            method_ = PUT;
        } else if (method == "DELETE") {
            method_ = GET;
        } else {
            method_ = INVALID;
        }
        return method_ != INVALID;
    }
    // PATCH OPTIONS CONNECT TRACE

    Method get_method() const { return method_; }
    void set_path(const char* start, const char* end) { path_.assign(start, end); }
    const std::string& get_path() const { return path_; }
    void set_query(const char* start, const char* end) { query_.assign(start, end); }
    const std::string& get_query() const { return query_; }

    // 添加 Header, such as Accept-Language: zh-CN 
    void AddHeader(const char* start, const char* colon, const char* end) {
        std::string field(start, colon);
        ++colon;  // :
        while (colon < end && isspace(*colon)) {
            ++colon;
        }
        std::string value(colon, end);
        while (!value.empty() && isspace(value[value.size()-1])) {
            value.resize(value.size()-1);  // 删去最后的空格
        }
        headers_[field] = value;
    }

    std::string GetHeader(const std::string& field) const {
        std::string result;
        std::map<std::string, std::string>::const_iterator it = headers_.find(field);
        if (it != headers_.end()) {
            result = it->second;
        }
        return result;
    }

    const std::map<std::string, std::string>& get_headers() const { return headers_; }

private:
    Method method_;  // 请求方法
    Version version_;  // 协议版本
    std::string path_;  // 请求路径
    std::string query_;  // 请求行 ?
    std::map<std::string, std::string> headers_;  // 请求报头 request header
};

}  // namespace yxalp

#endif  // YXALPHA_HTTPREQUEST_H_
