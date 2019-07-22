// yx
#include "httpcontext.h"
#include "utility/buffer.h"
#include "logger/logging.h"

namespace yxalp {

// GET /path?d&e HTTP/1._
bool HttpContext::ParseRequestLine(const char* begin, const char* end) {
    bool succeed = false;
    const char * start = begin;
    const char * space = std::find(start, end, ' ');
    if (space != end && request_.set_method(start, space)) {
        start = space +1;
        space = std::find(start, end, ' ');
        if (space != end) {  // none
            // std::cout << std::string(start, space) << std::endl;
            const char* question = std::find(start, space, '?');
            if (question != end) {
                request_.set_path(start, question);
                request_.set_query(question, space);
            } else {
                request_.set_path(start, space);  // 无参数
            }
            // version
            start = space + 1;
            succeed = (end - start == 8) && strncmp(start, "HTTP/1.", end - start);
            if (succeed) {
                char ch = *(end - 1);
                if ('1' == ch) {
                    request_.set_version(HttpRequest::HTTP11);
                } else if ('0' == ch) {
                    request_.set_version(HttpRequest::HTTP10);
                } else {
                    request_.set_version(HttpRequest::UNKNOWN);
                    succeed = false;
                }
            }
        }
    }
    return succeed;
}

bool HttpContext::ParseRequest(Buffer* buf) {
    bool ok = true;
    bool flag_end = false;
    while (!flag_end) {
        switch (state_) {
        case ExpectRequestLine: {
            const char * crlf = buf->findCRLF();
            if (crlf) {
                ok = ParseRequestLine(buf->peek(), crlf);
                if (ok) {
                    buf->move_read_index_until(crlf + 2);
                    state_ = ExpectHeaders;
                } else {
                    flag_end = true;  // 解析请求行失败
                }
            } else {
                flag_end = true;  // 无 \r\n
            }
            break;
        }
        case ExpectHeaders: {
            const char * crlf = buf->findCRLF();
            if (crlf) {
                const char * colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf) {
                    request_.AddHeader(buf->peek(), colon, crlf);
                } else {  // empty line
                    // TODO : body
                    state_ = Done;
                    flag_end = true;
                }
                buf->move_read_index_until(crlf + 2);
            } else {
                flag_end = false;
            }
            break;
        }
        case ExpectBody:
            // TODO 
            break;
        case Done:
            break;
        default:
            break;
        }
    }
    return ok;
}

}  // namespace yxalp
