// yx
#ifndef YXALPHA_TCPCONNECTION_H_
#define YXALPHA_TCPCONNECTION_H_

#include <memory>
#include <boost/any.hpp>

#include "utility/buffer.h"
#include "net/inetaddr.h"
#include "callback.h"

namespace yxalp {

class Dispatcher;
class EventHandler;
class Socket;

// 管理一个"已经建立"的连接
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    // TcpConnection(Dispatcher *dispatcher, Socket &&socket, 
    TcpConnection(Dispatcher *dispatcher, int fd, 
        const InetAddr &addr, const std::string &conn_name);
    ~TcpConnection();
    TcpConnection& operator=(const TcpConnection &) = delete;
    TcpConnection(const TcpConnection &) = delete;

    Dispatcher * get_dispatcher() { return dispatcher_; }
    const std::string & get_name() { return conn_name_; }
    const InetAddr & get_addr() { return addr_; }
    bool is_connected() const { return state_ == CONNECTED; }
    void set_context(const boost::any &context) { context_ = context; }
    const boost::any & get_context() const { return context_; }
    boost::any * get_context_ptr() { return &context_; }

    // 安全, 数据量较高时使用 outoutbuffer 机制, 但数据量过高时, 应当使用高水位回调
    void Send(const std::string &str);
    // 安全, 数据量较高时使用 outoutbuffer 机制, 但数据量过高时, 应当使用高水位回调
    void Send(Buffer *buf);
    // 直接将内部的 Buffer 数据发出, 危险操作, 数据量较少时, 可用来避免额外的内存分配操作
    void FlushOutputBuf();
    // 危险操作!
    Buffer * get_output_buffer() { return &output_buf_; }
    // 在写完所有数据之后, 就会关闭写端
    void ShutDown();

    // 用户与 TCPServer 都可以设置 CallBack
    void set_connection_call_back(const ConnectionCallback &cb)
    { connection_call_back_ = cb; }
    void set_message_call_back(const MessageCallback &cb)
    { message_call_back_ = cb; }
    // 低水位回调
    void set_write_complete_callback(const WriteCompleteCallback & cb) 
    { write_complete_callback_ = cb; }
    // 高水位回调, 只上升沿边触发一次, high_water_mark 为警戒线
    void set_high_water_mark_callback(const HighWaterMarkCallback &cb, size_t high_water_mark) 
    { high_water_mark_callback_ = cb; high_water_mark_ = high_water_mark;}
    // 内部使用, 只应该由 server 调用删除
    void set_close_call_back(const CloseCallBack & cb)
    { close_call_back_ = cb; }
    // 连接已建立, 只应该由 server 设置. 会直接设置监听状态, 并回调用户函数
    void set_connection_established();
    // 连接已断开, 此时TCPServer 与 TcpClient 必须"已经"移除自己持有的本对象
    void set_connection_destroyed();

private:
    enum ConnState {CONNECTING, CONNECTED, DISCONNECTING, DISCONNECTED};

    void set_state(ConnState cs) { state_ = cs; }
    void HandleRead();
    void HandleWrite();
    void HandleError();
    void HandleClose();
    void SendInLoop(const char * data, size_t len);
    void ShutDownInLoop();
    void Flush_();

    Dispatcher *dispatcher_;
    ConnState state_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<EventHandler> event_handler_;  // 用于到 Dispatcher 上注册监听的 Handler
    InetAddr addr_;
    std::string conn_name_;
    ConnectionCallback connection_call_back_;
    MessageCallback message_call_back_;
    CloseCallBack close_call_back_;
    size_t high_water_mark_;
    WriteCompleteCallback write_complete_callback_;
    HighWaterMarkCallback high_water_mark_callback_;
    Buffer input_buf_;
    Buffer output_buf_;
    boost::any context_;
};

}  // namespace yxalp

#endif  // YXALPHA_TCPCONNECTION_H_