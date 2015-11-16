#ifndef __SINGLTONE_H_
#define __SINGLTONE_H_
#pragma once
#include "MAssert.h"
template < class T >
class Singltone {
public:
    static inline T*  GetInstance() {return g_instance;}
    void Release();
    virtual ~Singltone();
    Singltone();
    Singltone(const Singltone<T>& c ) = delete;

    template<typename... Args> Singltone(Args... args);

private:
    static T* g_instance;
};
/*Init*/
template < class T >
T* Singltone<T>::g_instance = 0;
template < class T >
void Singltone<T>::Release()
{
    if (this)
    {
        delete this;
        g_instance = 0;
    } else {
        /*ASSERT should be there*/
    }
}
template < class T>
Singltone<T>::~Singltone()
{
    g_instance = 0;
}
template < class T>
template < typename... Args>
Singltone<T>::Singltone(Args... args)
{
    if (!g_instance)
        g_instance = new T(args...);
    else {
        /*ASSERT should be there */
    }
}
template < class T>
Singltone<T>::Singltone()
{
    if (!g_instance)
        g_instance = new T();
    else {
        MASSERT(true); /*new singltone instances shouldn't have arguments */
    }
}
#endif
