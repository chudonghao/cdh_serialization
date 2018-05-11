//
// Created by chudonghao on 18-5-20.
//

#ifndef CDH_SERIALIZATION_PROCESSER_H
#define CDH_SERIALIZATION_PROCESSER_H

#include <type_traits>
#include <functional>
#include <map>

namespace cdh {
namespace serialization {

class serializer;
class deserializer;

template<typename Processer, typename T>
void process(Processer &ar, T &v);

template<typename Processer, typename T, class Enable = void>
class process_func;

template<typename Processer, typename T>
class process_func<Processer, T,
        typename std::enable_if<std::is_base_of<serializer, Processer>::value>::type> {
public:
    Processer &
    operator()(Processer &ar, void const *v) {
        process(ar, *const_cast<T *>(static_cast<T const *>(v)));
        return ar;
    }
};

template<typename Processer, typename T>
class process_func<Processer, T,
        typename std::enable_if<std::is_base_of<deserializer, Processer>::value>::type> {
public:
    Processer &
    operator()(Processer &ar, void const *v) {
        process(ar, *const_cast<T *>(static_cast<T const *>(v)));
        return ar;
    }
};

template<typename Processer>
class process_funcs : protected std::map<const std::string, std::function<Processer &(Processer &, void const *)> > {
    friend Processer;
    typedef process_funcs<Processer> self_t;
public:
    template<typename T>
    typename std::enable_if<std::is_class<T>::value, self_t &>::type
    enable_rtti() {
        typedef typename std::decay<T>::type base_t;
        typedef typename std::add_lvalue_reference<base_t>::type reference_t;
        typedef typename std::add_const<reference_t>::type const_reference_t;
        typedef typename std::add_pointer<base_t>::type pointer_t;
        (*this)[typeid(const_reference_t).name()] = process_func<Processer, base_t>();
        return *this;
    }

    //template<typename T>
    //typename std::enable_if<std::is_class<T>::value, self_t &>::type
    //enable_rtti(char const *type_name) {
    //    typedef typename std::decay<T>::type base_t;
    //    typedef typename std::add_lvalue_reference<base_t>::type reference_t;
    //    typedef typename std::add_const<reference_t>::type const_reference_t;
    //    typedef typename std::add_pointer<base_t>::type pointer_t;
    //    (*this)[type_name] = process_func<Processer, base_t>();
    //    return *this;
    //}
};

}
}

#endif //
