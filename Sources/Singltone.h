template < class T >
class Singltone {
public:
    static T* GetInstance() {return g_instance;}
    void Release();
    virtual ~Singltone();
    Singltone();

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
Singltone<T>::Singltone()
{
    if (!g_instance)
        g_instance = (T*)this;
    else {
        /*ASSERT should be there */
    }
}
