
## 设计

### threadpool
一个基于生产者-消费者任务队列模式的线程池.

线程池处于未激活状态下, 则由任务添加者自己运行任务.

任务队列使用了一个 deque, 跨线程对象, pop/push 使用 mutex 保护. 两个条件变量 cond_full_ 与 cond_empty_ 分别表示队列的空与满状态. 这是一个阻塞队列.

使用 function/bind 处理用户任务以及线程对象.

条件变量 cond_empty_ 有两个用途:
1. 标识任务队列是否为空. queue_ 为空时, 工作线程会阻塞在该条件变量上, 由生产者添加新任务后进行 signal.
2. 线程池 stop 时, 需要 notifyAll 阻塞在 cond_empty_ 上的消费者线程, 此时任务队列为空, 不能pop_front(), 工作线程会得到一个空 function 对象.

stop时, 不管任务队列中还有没有任务, 直接退出, 回收所有 worker threads.

## 组件

### __thread

#### 概念
__thread是GCC内置的线程局部存储设施。_thread变量每一个线程有一份独立实体，各个线程的值互不干扰。可以用来修饰那些带有全局性且值可能变，但是又不值得用全局变量保护的变量。
1. __thread是GCC内置的线程局部存储设施
2. 只能修饰POD类型(类似整型指针的标量，不带自定义的构造、拷贝、赋值、析构的类型，二进制内容可以任意复制memset,memcpy,且内容可以复原).
3. 不能修饰class类型，因为无法自动调用构造函数和析构函数.
4. 可以用于修饰全局变量，函数内的静态变量，不能修饰函数的局部变量或者class的普通成员变量，且__thread变量值只能初始化为编译器常量.
链接：
https://www.jianshu.com/p/997b533842c8
https://www.jianshu.com/p/495ea7ce649b

#### 用处
使用了两个全局变量, 这样每个 thread 都有一份属于自己的全局变量, 作为线程标识.
```C++
    __thread tid_t t_tid = 0;  //  线程标识
    __thread const char* t_name = "unknown";  // 线程名称
```

## Other
* [一个死锁](./一个死锁), 蛮有趣的 : )