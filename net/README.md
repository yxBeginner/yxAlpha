# Net

## Acceptor
用于接受新连接. TcpServer 的成员.
仍然是基于对象风格, Acceptor 本身是一个 EventHandler + server socket, 但并没有继承二者, 而是设置了 EventHandler 的回调. 相当于继承并使用了二者的一部分功能, 重写了一部分功能. 每当有新连接到达时, Acceptor 的回调函数便会被触发, 


## TcpConnection
在 EventHandler 上监听读写事件. 读事件会通过 message_call_back_ 回调客端函数. 客端通过 Connection 写, 随后被发送(自动的处理).

管理一个"已经建立"的连接
