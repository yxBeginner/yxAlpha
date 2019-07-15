# Net

## Acceptor
用于接受新连接. TcpServer 的成员.
仍然是基于对象风格, Acceptor 本身是一个 EventHandler + server socket, 但并没有继承二者, 而是设置了 EventHandler 的回调. 相当于继承并使用了二者的一部分功能, 重写了一部分功能. 每当有新连接到达时, Acceptor 的回调函数便会被触发, 

## TcpConnection
管理一个"已经建立"的连接
在 EventHandler 上监听读写事件. 读事件会通过 message_call_back_ 回调客端函数. 客端通过 Connection 写, 随后被发送(自动的处理).

### TcpConnecion 的 4 个状态:
* CONNECTING : 初始被构造时的状态, 标识正在建立连接 (实际上此时已经是是 accept 返回的连接了, 只是 server 上需要一些处理), 当设置完毕, 最后 TcpServer 会调用 set_connection_established(), 进入下一状态.
* CONNECTED : 连接已建立, 正常读写. TcpServer 在 newConnection 的最后会调用设置该状态.
* DISCONNECTING : 用户层主动调用 shutdown, 此时关闭本端的写, 发送 FIN 分节.  如果应用层调用时, 还有数据在 Buffer 中, 那么会等到 handleWrite() 中将数据全部写完, 才会真正 shutdown.
* DISCONNECTED : 已经处于几乎关闭的状态, 任何写/读都不在有效, 最后处理完 routine 就 close.

### 发送数据
一切都是尽可能的提升效率.
1. 如果不是已经在监听 fd 的写状态, 那么首先尝试直接 write, 而不是直接注册写事件, 因为这样做 epoll_wait 肯定会会立即返回, 数据量较小的情况下, 多耗用了数次系统调用.
2. 第一次没有写完全部, 不在尝试第二次, 此时内核 buffer 几乎仍然是满的, 一次数据的成功送到对端, 几乎必然比连续两次用户层调用 write 慢点多的多, 白白返回一次 EAGAIN 只是浪费时间. 
3. 没有写完, 此时再去注册 write 事件, 等待 epoll_wait 返回.
4. 此中需要注意跨线程的问题, TCPConnection 的持有者并不明确, 但是第一次尝试 send 只能在该连接所在的线程执行(内部保证).

### 各类回调函数
* connection_call_back_ : 表示连接上有变化, 断开/建立连接时都会回调此函数. 用户应当使用 is_connected() 检查这是哪一个事件.
* message_call_back_ : 新数据到达时的回调, 也即 epoll_wait 返回 socket 可读.
* write_complete_callback_ : 低水位回调. 如果用户设置了这一回调, 那么在每次 write(1) 所有数据, 以及 TcpConnection 的 Output Buffer 被清空之后, 都会回调这一函数 . 如果发送数据量巨大, 则不应当直接反复使用 Send() 直接发送数据, 而是在 write_complete_callback_ 回调中继续发送. Output Buffer 默认没有限制大小, 如果对端数据接收过慢, 则本端的 Buffer 会不停占用更多空间. 此外,  write_complete_callback_ 意味着写到了内核缓冲区中, 而不是对端成功接收到了数据.
* high_water_mark_callback_ : 高水位回调, 只上升沿边触发一次
* close_call_back_ : 内部使用, 只应该由 server 调用删除
