// yx
#ifndef YXALPHA_CALLBACK_H_
#define YXALPHA_CALLBACK_H_

#include <memory>
#include <functional>

namespace yxalp {

class TcpConnection;
class Buffer;

typedef std::function<void()> TimerCallBack;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
// 收到新 TCP 连接时的回调函数, 参数是建立的连接
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
// 收到新消息后, 执行该回调函数.
typedef std::function<void (const TcpConnectionPtr&, Buffer *buf)> MessageCallback;
// 对端 close 的回调函数
typedef std::function<void (const TcpConnectionPtr &)> CloseCallBack;
typedef std::function<void (const TcpConnectionPtr &)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr &, size_t)> HighWaterMarkCallback;

}  // namespace yxalp

#endif  // YXALPHA_CALLBACK_H_
