#ifndef __YXALPHA_SINGLETON_H__
#define __YXALPHA_SINGLETON_H__

#include <pthread.h>

// 通用Singleton, 只能支持默认构造函数, 不考虑析构.
// usage:  Foo& foo = Singleton<foo>::getInstance();
template<typename T>
class Singleton {
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator= (const Singleton&) = delete;

    static T& getInstance() {
        pthread_once(&once_, Singleton::init());
        return *value_;
    }

private:
    Singleton() {};
    ~Singleton() {};

    static void init() {
        value_ = new T;
    }

    static pthread_once_t once_;
    static T* value_;
};

template<typename T> pthread_once_t Singleton<T>::once_ = PTHREAD_ONCE_INIT;
template<typename T> T* Singleton<T>::value_ = nullptr;

#endif