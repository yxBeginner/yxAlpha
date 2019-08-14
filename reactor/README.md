# Reactor

结构主体是**事件循环**, 由**事件驱动**(epoll), 执行**事件回调**. -- `事件分发机制`.

> 基于事件编程: non-blocking IO + IO multipleing, 程序主体是被动的等待事件发生, 事件发生后网络库会回调事先注册的事件处理函数(Event Handler).

## Component

### Event Handler
事件处理器. EventHandler 会定义一些回调方法 (钩子函数)，当所管理的 fd 上有事件发生时，回调方法便会执行(由 Dispatcher 回调)，一种事件处理机制.
* EventHandler 定义事件处理方法 `HandleEvent()`，以供 Dispatcher 处理事件。经由若干 set 方法设置客端回调(通常由 TcPServer 设置相应的回调), 并可改变监听状态 (一个 Channel). 主要有:
  * `read_func_` (readCallBack): 可读 | 急迫数据可读(带外数据) | 关闭/半关闭(EPOLLIN | EPOLLPRI | EPOLLRDHUP) 状态下会被触发. 
  * `write_func_` (writeCallBack): 可写 (EPOLLOUT), 写数据不会阻塞.
  * `error_func_` : 错误(EPOLLERR).
  * `close_func_` : peer 关闭 && 无数据可读 ((EPOLLHUP) && !(revents_ & EPOLLIN )). 主要为 TcpServer 使用, 用于关闭连接, 移除持有的 ptr. 这个 closeCallBack 不是唯一的关闭连接情况, 在 TcpConnection 中 read 返回 0 的话也会回调 HandleClose.
* 每个 EventHandler 只隶属于一个 IO 线程, 非跨线程(但在创建时, 可能跨线程). 它只是一个监听事件的渠道, 不管理 fd 的生命周期. 作为 Acceptor/TCPConnection/Dispatcher(eventfd) 的成员.
* EventHanlder 不用于派生 Concrete EventHandler. 期望监听者通过设置回调函数来获得响应事件, 并被执行相应函数, BO风格.
* 构造 EventHandler 后, 需要注册调用者的 Dispatcher, 以便 EventHandler 到相应的 Dispatcher 上改变自己关心的事件.

### Selector
所谓的 Synchronous Event Demultiplexer, 也即 IO multiplexing 的核心, 内部使用 epoll LT 实现. 与 Dispatcher 为一对一的关系.
*  Selector 缓存 epoll_wait 返回的被激活的 fd 数组, 进行错误处理, 然后扫描并设置 EventHandler 上的激活事件, 将不同的事件分发为不同的回调. ~~(但并不直接调用 EventHandler 上的回调). 并将 fd(EventHandler) 数组返还 Dispatcher 进一步处理.~~
更新: 在 selector 处直接进行了事件的分发与回调, 没有转移至 DIspatcher, 避免了一步额外开销.
* 封装了 ADD/DEL/MOD 等动作.

### Dispatcher
分发器，整个 Reactor 结构的交汇处，提供了注册、删除与转发EventHandler的方法。
实际实现中, EventHandler 提供了单独的接口, 它记录自身的状态. 除了用于异步唤醒的 eventfd 相关的 EventHandler, Dispatcher 中不在持有任何与 EventHandler相关的结构. ~~Dispatcher 中只持有 Selector 返回的激活事件列表, 并逐一处理 Handler 上的事件.~~
更新: Dispatcher 不在持有激活事件的数组, 这一步处理被下放到了 Selector 中.

目前 Dispatcher 中存在的主要功能, 是为了 OneLoop Per Thread 结构而提供的, 比如 RunInLoop/QueueInLoop; 或者是业务的正常开始与结束 Loop/Quit 等.

### Reactor 结构上与 Muduo 的差异
以下陈述中, 激活事件数目为 na, 所有存在的连接事件是 nb. 
只针对两者的 epoll 管理方式上:
1. 在返回激活事件的处理上, Muduo 线性拷贝了一次激活的事件数组, 然后对激活的事件 channel 添加到返回给上一层时使用的是 push_back, 这个的话性能差一点, 比如说刚开始的时候, 大规模添加新事件的状况, 或者流量剧增, 直接 push_back 都不是一个好的操作, 哪管先resize一下也好. 无论如何 clear 也不会缩小空间... 再说了, 我们这个测试情况下, 所有事件一直都是激活的.
2. 在上层封装处, 第二次遍历了这个激活事件数组, 又是一步 O(na). 然后逐一回调相应的事件.
3. 它在用户态层外维护了一个 map, 用于管理 channel 对象, 这样的话, 增删操作(更新的话不需要) 是两个 O(lg na) 操作. 不过这个也是有原因的吧, 暂时不监听任何操作的情况下, Muduo 直接将内核中相应的 fd 设置为 -1(或者相反数啥的), 否则依然会返回出错信息. yxAlpha 直接取消监听任一事件, 所以可能会有些问题(我想更好的做法其实应当是去内核中修改 epoll 处理逻辑, 而不是在内核与用户层分别管理一次).

与 poller 结构对比:(待补充) poll 因为需要在用户态维持一个 pollfd 数组, 所以在用户层上要进行大量的线性操作. 此外为了安全, 还不能在遍历返回事件数组中回调相应的函数, 因为可能会向 pollfds 中增删 fd.

ps: 对于使用 ab 工具等的测试场景来说的话, 所有事件都是激活的, 所以 na=nb.
总结: 所有面向跨平台的/通用性的库的共性吧, 为了屏蔽这些差异, 而牺牲了一些速度. 但是当连接规模/吞吐量变大的时候, 牺牲的哪怕一点性能就会被随之扩大.

## 说两句
Reactor 模式是在单线程/进程下并发度吞吐量最佳的模式之一, 当然也可以扩展成为多线程模式工作. IO, 连接的建立/接收 都可以非阻塞的进行, 非常适用于IO密集型的应用. Lighttpd, ACE, NIO, libevent, Redis 都实现了这一模式.

缺点也有, 这种风格编写程序, 首先不能出现阻塞的任务, 否则结构主体与其他任务会饥饿. 而且不是很适合有复杂逻辑的上层应用, 一般连接建立, 收发数据等都要写在不同的回调函数中, 存在着很严重的割裂逻辑问题, 更适合于非繁琐型的应用. 不适合计算任务重的业务, 因为耗时+IO少, 更适合使用额外的线程单独处理计算, 而不是在 Reactor 中处理.

## Design
遵循 one Loop per thread, 每个线程只有一个 Dispatcher (EventLoop in Muduo), 一个 Dispatcher 也只属于一个线程. 跨线程的客端函数执行经由 Dispatcher 的接口进行, RunInLoop() / QueueInLoop(). 这个二者是一个任务队列, 前者对后者进行了封装, 跨线程调用时由内部互斥锁保护, 并经由 eventfd 异步唤醒. 内部具体的队列在本 Dispatcher 处理回调函数时直接与局部队列 swap, 缩小临界区.

使用 epoll_event.data.ptr 指示该 fd 所处于的 EventHandler, 整个 Reactor 结构中没有维护任何存活的 EventHandler 与 文件描述符的数据, 皆由Kernel 处理. (ps. TcpServer 中维护了 EventHandler 的列表)

## Other Stuff

### Epoll 返回状态的处理

#### `EPOLLHUP`
man 手册解释:
挂起发生在关联的文件描述符上。
epoll_wait(2)将始终等待此事件;没有必要在事件中设置它。
请注意，当从管道或流套接字等通道读取时，此事件仅指示对等端关闭了通道的末端。只有在通道中所有未完成的数据都被使用之后，对通道的后续读取才会返回0(文件结束)。

EventHandler 中检测如下条件:
```C++
    if ((revents_ & (EPOLLHUP)) && !(revents_ & EPOLLIN ))
```
这意味着对端关闭了连接, 又无数据可读, 此时调用客户的 close 历程.
