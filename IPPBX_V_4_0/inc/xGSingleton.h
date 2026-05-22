#ifndef __XGSINGLETON_H__
#define __XGSINGLETON_H__
#pragma once

#include <memory>

using namespace std;

// Mayers Singleton class template
template <typename T>
class xGSingleton
{
public:
    static T &instance();
    xGSingleton(const T &) = delete;
    xGSingleton &operator=(const T &) = delete;
    xGSingleton(T &&) = delete;
    xGSingleton &operator=(T &&) = delete;

protected:
    struct token
    {
    };
    xGSingleton() = default;
}; // end of 'XGSingleton' class

template <typename T>
inline T &xGSingleton<T>::instance()
{
    static const std::unique_ptr<T> instance{new T{token{}}};
    return *instance;
}

#endif //__XGSINGLETON_H__
