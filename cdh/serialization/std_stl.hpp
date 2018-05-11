#ifndef CDH_SERIALIZATION_STD_STL_H
#define CDH_SERIALIZATION_STD_STL_H

#include <vector>
#include <list>
#include "processer.hpp"

namespace cdh {
namespace serialization {

template<typename Processer, typename T>
typename std::enable_if<std::is_base_of<serializer, Processer>::value>::type
process(Processer &ar, std::vector<T> &c) {
    auto i = c.size();
    ar & make_nvp("size", i);
    for (typename std::vector<T>::iterator iter = c.begin(); iter != c.end(); ++iter) {
        ar & make_nvp("item", *iter);
    }
};

template<typename Processer, typename T>
typename std::enable_if<std::is_base_of<serializer, Processer>::value>::type
process(Processer &ar, std::list<T> &c) {
    auto i = c.size();
    ar & make_nvp("size", i);
    for (typename std::list<T>::iterator iter = c.begin(); iter != c.end(); ++iter) {
        ar & make_nvp("item", *iter);
    }
};

template<typename Processer, typename T>
typename std::enable_if<std::is_base_of<deserializer, Processer>::value>::type
process(Processer &ar, std::vector<T> &c) {
    unsigned int i = 0;
    ar & make_nvp("size", i);
    c.resize(i);
    for (int j = 0; j < i; ++j) {
        ar & make_nvp("item", c[j]);
    }
};

template<typename Processer, typename T>
typename std::enable_if<std::is_base_of<deserializer, Processer>::value>::type
process(Processer &ar, std::list<T> &c) {
    unsigned int i = 0;
    ar & make_nvp("size", i);
    for (int j = 0; j < i; ++j) {
        c.emplace_back();
        ar & make_nvp("item", c.back());
    }
};

}
}

#endif
