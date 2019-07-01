# Log

## 技术点

1. 前后端分离. 前端使用 logstream 进行行缓冲, 每条日志都创建一份单独的 buffer, 并且控制格式转换等相关内容. 后端利用单独的线程负责将日志信息写到文件中.

2. 使用了双缓冲技术, 前端与后端使用不同的 buffer, 必要时进行切换. 避免争用.

## 组件

### move
移动语义, 
对于智能指针来说, 如 shared_ptr, 此动作直接转移对象的所有权, 被转移者不管理对象. 相当于 reset( ).
前后端的 buffer 交换是利用 move 交换 share_ptr 所管理的资源.

**待完善**
## 测试
开启 4 个测试线程 + 主线程, 一起不间断的写 log, 后端输出到文件中. 写入磁盘的速度大概在 110 M/s 上下, 但此时 CPU 利用率远还没有达到 100%. 需要进一步测试.
!["test"](../not_code/log_test2.png)