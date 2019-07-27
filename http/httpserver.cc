// yx

#include "httpserver.h"
#include "httpresponse.h"
#include "httprequest.h"
#include "httpcontext.h"
#include "logger/logging.h"

namespace yxalp{

static void response_503(const TcpConnectionPtr& conn);

void DefaultHttpCallBack(const HttpRequest& require, HttpResponse* response) {
    response->set_status_code(HttpResponse::NotFound);
    response->set_status_message("Not Found");
    response->set_close_connection(true);
}

HttpServer::HttpServer(Dispatcher *disaptcher, const InetAddr &addr, int32_t max_conn) 
     : server_(disaptcher, addr),
       http_call_back_(DefaultHttpCallBack),
       num_conn_(0),
       max_conn_(max_conn) {
    server_.set_connection_call_back(
        std::bind(&HttpServer::OnConnection, this, std::placeholders::_1));
    server_.set_message_call_back(
        std::bind(&HttpServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
}

void HttpServer::Start() {
    LOG << "HttpServer::Start() start";
    server_.Start();
}

void HttpServer::OnConnection(const TcpConnectionPtr& conn) {
    DLOG << "HttpServer::OnConnection " << conn->get_name().c_str();
    if (conn->is_connected()) {
        conn->set_context(HttpContext());  // set ?
        ++num_conn_;
        if (num_conn_ > max_conn_) {
            response_503(conn);
        }
    } else {
        --num_conn_;
    }
}

void HttpServer::OnMessage(const TcpConnectionPtr& conn, Buffer* buf) {
    HttpContext* context = boost::any_cast<HttpContext>(conn->get_context_ptr());
    // std::cout << buf->GetStringNoMove() << std::endl;
    if (!context->ParseRequest(buf)) {  // 解析失败
        conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->ShutDown();
    }

    if (context->is_done()) {  // 请求消息解析完毕
        OnRequest(conn, context->get_request());
        context->Reset();  // 本次请求处理完毕，重置HttpContext，适用于长连接
    }
}

// 其中parseRequest() 会将存放在Buffer 中的请求解析到
// server_.TcpConnection.context_.request_ 中，最后调用HttpServer::onRequest()
void HttpServer::OnRequest(const TcpConnectionPtr& conn, const HttpRequest& req) {
    const std::string& connection = req.GetHeader("Connection");
    bool close = connection == "close" ||
        (req.get_version() == HttpRequest::HTTP10 && connection != "Keep-Alive");
    HttpResponse response(close);  // 构造响应
    http_call_back_(req, &response);  // 客户代码设置的回调函数，填充response
    Buffer buf;
    response.AppendToBuffer(&buf);
    conn->Send(&buf);  // 将buf 中的响应发送给客户端
    //如果非Keep-Alive则直接关掉
    if (response.is_close_connection()) {
        conn->ShutDown();
    }
}

static void response_503(const TcpConnectionPtr& conn) {
    HttpResponse response(true);
    response.set_status_code(HttpResponse::ServiceUnavailable);
    response.set_status_message("Service Temporary Unavailable");
    response.set_content_type("text/html");
    response.add_header(std::string("Server"), std::string("Yxalp"));
    response.set_body("<html><head><title>Yxalp</title></head>"
    "<body><h1>Service Temporary Unavailable</h1></body></html>");
    Buffer buf;
    response.AppendToBuffer(&buf);
    conn->Send(&buf);
    conn->ShutDown();
}

}  // namespace yxalp
