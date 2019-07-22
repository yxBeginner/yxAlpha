// yx
#ifndef YXALPHA_HTTPSERVER_H_
#define YXALPHA_HTTPSERVER_H_

#include "net/tcpserver.h"

namespace yxalp {

class HttpRequest;
class HttpResponse;

class HttpServer {
public:
    typedef std::function<void (const HttpRequest&, HttpResponse*)> HttpCallBack;

    HttpServer(Dispatcher *disaptcher, const InetAddr &addr);

    Dispatcher * get_dispatcher() const { return server_.get_dispatcher(); }

    void set_call_back(const HttpCallBack &cb) { http_call_back_ = cb; }

    void SetThreadNum(int num_threads) { server_.SetThreadNum(num_threads); }

    void Start();
private:
    void OnConnection(const TcpConnectionPtr& conn);
    void OnMessage(const TcpConnectionPtr& conn, Buffer* buf);
    void OnRequest(const TcpConnectionPtr&, const HttpRequest&);
    TcpServer server_;
    HttpCallBack http_call_back_;
};

}  // namespace yxalp

#endif  // YXALPHA_HTTPSERVER_H_