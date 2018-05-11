//
// Created by chudonghao on 18-5-20.
//

#ifndef CDH_SERIALIZATION_SINGLETON_H
#define CDH_SERIALIZATION_SINGLETON_H

namespace cdh{
namespace serialization{

template <class T>
class singleton : private T
{
private:
    singleton();
    ~singleton();

public:
    static T &instance();
};


template <class T>
inline singleton<T>::singleton()
{
    /* no-op */
}

template <class T>
inline singleton<T>::~singleton()
{
    /* no-op */
}

template <class T>
/*static*/ T &singleton<T>::instance()
{
    // function-local static to force this to work correctly at static
    // initialization time.
    static singleton<T> s_oT;
    return(s_oT);
}

}
}

#endif //CDH_SERIALIZATION_SINGLETON_H
