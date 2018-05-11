#ifndef CDH_SERIALIZATION_NVP_H
#define CDH_SERIALIZATION_NVP_H

#include <type_traits>
#include <map>

namespace cdh {
namespace serialization {

/**
 * 借鉴了boost::serialization::nvp
 */
template<typename T,bool is_base_object = false>
class nvp;

template<typename Base>
class base_object {
public:

    template<typename Derived>
    base_object(Derived &derived):base(static_cast<Base &>(derived)) {}

    Base &base;

    Base &reference() { return base; }
};

template<typename T>
typename std::enable_if<std::is_const<T>::value, nvp<T>>::type
make_nvp(char const *name, T &t);

template<typename T>
typename std::enable_if<!std::is_const<T>::value, nvp<T>>::type
make_nvp(char const *name, T &t);

template<typename T>
typename std::enable_if<true, nvp<T,true>>::type
make_nvp(char const *name, base_object<T> &&t);

template<typename T, bool is_base_object>
class nvp {
public:
    typedef typename std::decay<T>::type base_t;
    typedef typename std::add_lvalue_reference<base_t>::type reference_t;
    typedef typename std::add_const<reference_t>::type const_reference_t;
    typedef typename std::add_pointer<base_t>::type pointer_t;
private:

    std::pair<char const *, pointer_t> pair_;

    nvp(char const *name, reference_t t) : pair_(name, &t) {}

public:

    friend
    nvp<T>
    make_nvp<T>(char const *name, T &t);

    friend
    nvp<T,true>
    make_nvp<T>(char const *name, base_object <T> &&t);

    char const *name() const {
        return pair_.first;
    }

    reference_t data_reference() const {
        return *pair_.second;
    }

    const_reference_t const_data_reference() const {
        return *pair_.second;
    }

};

template<typename T>
typename std::enable_if<!std::is_const<T>::value, nvp<T>>::type
make_nvp(char const *name, T &t) {
    typedef typename std::decay<T>::type base_t;
    return nvp<base_t>(name, t);
}

template<typename T>
typename std::enable_if<std::is_const<T>::value, nvp<T>>::type
make_nvp(char const *name, T &t) {
    //TODO
    throw std::runtime_error("const type is not supported yet. !TODO");
}

template<typename T>
typename std::enable_if<true, nvp<T,true>>::type
make_nvp(char const *name, base_object<T> &&t) {
    typedef typename std::decay<T>::type base_t;
    return nvp<base_t,true>(name, t.reference());
}

}
}
#endif
