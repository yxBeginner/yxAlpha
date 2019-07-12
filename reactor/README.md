
# Reactor

non-blocking IO + IO multipleing

基本结构为一个事件循环, 事件驱动/ 事件回调

> 基于事件编程: 程序主体是被动的等待事件发生, 事件发生后网络库会回调事先注册的事件处理函数(Event Handler).

## Component

### Event Handler
事件处理器，其会定义一些回调方法 (钩子函数)，当所管理的 fd 上有事件发生时，回调方法便会执行(由 Dispatcher 回调)，一种事件处理机制.
* EventHandler 定义事件处理方法 HandleEvent()，以供 Dispatcher 回调使用。经由若干 set 方法设置客端回调, 并可改变监听状态 (一个 Channel).
* 每个 EventHandler 只隶属于一个 IO 线程, 非跨线程. 并且不管理 fd 的生命周期. 作为 Acceptor/TCPConnection/Dispatcher 的成员.
* EventHanlder 不用于派生 Concrete EventHandler, 而是通过设置回调函数的方式提供多态, 这是基于对象风格?
* 构造 EventHandler 后, 需要注册调用者的Dispatcher, 以便 EventHandler 到 相应的 Dispatcher 上改变自己关心的事件. 考虑到 Dispatcher 不会正常退出, 那么这个引用是安全的.

### Selector
所谓的 Synchronous Event Demultiplexer, 也即 IO multiplexing 的核心, 内部使用 epoll LT 实现. 与 Dispatcher 为一对一的关系.
*  Selector 缓存 epoll_wait 返回的被激活的 fd 数组, 进行错误处理, 然后扫描并设置 EventHandler 上的激活事件(但并不直接调用 EventHandler 上的回调). 并将 fd(EventHandler) 数组返还 Dispatcher 进一步处理.
* 此外封装了 ADD/DEL/MOD 等动作.

### Dispatcher
分发器，整个 Reactor 结构的交汇处，提供了注册、删除与转发EventHandler的方法。
实际实现中, EventHandler 提供了单独的接口, 它记录自身的状态, Dispatcher 中只持有 Selector 返回的激活事件列表, 并逐一处理 Handler 上的事件.

以下与 Reactor 模型无关:
遵循 one Loop per thread, 每个线程只有一个 Dispatcher (EventLoop in Muduo), 一个 Dispatcher 也只属于一个线程. 跨线程的客端函数执行经由 Dispatcher 的接口进行, RunInLoop() / QueueInLoop(). 这个二者是一个任务队列, 前者对后者进行了封装, 跨线程调用时由内部互斥锁保护, 并经由 eventfd 异步唤醒. 内部具体的队列在本 Dispatcher 处理回调函数时直接与局部队列 swap, 缩小临界区.

## Design

使用 epoll_event.data.ptr 指示该 fd 所处于的 EventHandler, 整个 Reactor 结构中没有维护任何存活的 EventHandler 与 文件描述符的数据, 皆由Kernel 处理. (ps. TcpServer 中维护了 EventHandler 的列表)

## Epoll 返回状态的处理

### `EPOLLHUP`
man 手册解释:
挂起发生在关联的文件描述符上。
epoll_wait(2)将始终等待此事件;没有必要在事件中设置它。
请注意，当从管道或流套接字等通道读取时，此事件仅指示对等端关闭了通道的末端。只有在通道中所有未完成的数据都被使用之后，对通道的后续读取才会返回0(文件结束)。

EventHandler 中检测如下条件:
```C++
    if ((revents_ & (EPOLLHUP)) && !(revents_ & EPOLLIN ))
```
这意味着对端关闭了连接, 又无数据可读, 此时调用客户的 close 历程.
