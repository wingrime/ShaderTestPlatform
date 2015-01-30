#pragma once
template < class T >
class Singltone {
public:
    static inline T*  GetInstance() {return g_instance;}
    void Release();
    virtual ~Singltone();
    Singltone(T *inst);

private:
    static T* g_instance;
};
/*Init*/
template < class T >
T* Singltone<T>::g_instance = nullptr;
template < class T >
void Singltone<T>::Release()
{
    if (this)
    {
        delete this;
        g_instance = nullptr;
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
Singltone<T>::Singltone(T* inst)
{
    if (!g_instance)
        g_instance = inst;
    else {
        /*ASSERT should be there */
    }
}
