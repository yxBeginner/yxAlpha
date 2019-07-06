// yx
#ifndef YXALPHA_CALLBACK_H_
#define YXALPHA_CALLBACK_H_

#include <memory>
#include <functional>

namespace yxalp {

class TcpConnection;

// 使用 shared_ptr 包裹的 TCPConnection, 减轻资源管理的负担
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
// 收到新 TCP 连接时的回调函数, 参数是建立的连接
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
// 收到新消息后, 执行该回调函数.
typedef std::function<void (const TcpConnectionPtr&, const char* data, ssize_t len)> MessageCallback;
// 对端 close 的回调函数
typedef std::function<void (const TcpConnectionPtr &)> CloseCallBack;
}  // namespace yxalp

#endif  // YXALPHA_CALLBACK_H_
