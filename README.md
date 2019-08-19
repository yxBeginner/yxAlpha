# Yxalp
一个基于 Reactor 模式的网络库, 仿照 [muduo](https://github.com/chenshuo) 而写.

## 组件
* [http](./http) : 一个简单的 http 模块, 可扩展. httptest.cc 是一个简单的解析 GET 请求测试.
* [logger](./logger) : 基于写回策略. 后端线程周期性的唤醒, 或者在前端 log 数据量巨大的时候被主动唤醒, 将 buffer 中的数据刷出到磁盘上.
* [net](./net) : 网络连接封装.
* [reactor](./reactor) : non-blocking IO + IO multipleing 模式, 基于事件编程的核心.
* [test](./test) 一些整体测试(old).
* [thread](./thread) RAII 封装的互斥锁设施以及相关部件. 实现了线程对象以及任务队列形式的线程池.
* [time](./time) 时间戳相关组件, 部分完成.
* [utility](./utility) 额外部件.

## 开发平台
* OS: Ubuntu 16.04 xenial
* Kernel: x86_64 Linux 4.15.0-36-generic
* CPU: Intel Core i3-4160 CPU @ 3.6GHz
* RAM: 7848MiB

*******
 There is still a long way to go.
