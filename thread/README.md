


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