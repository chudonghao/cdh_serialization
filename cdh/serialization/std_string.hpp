//
// Created by chudonghao on 18-5-22.
//

#ifndef CDH_SERIALIZATION_STD_STRING_H
#define CDH_SERIALIZATION_STD_STRING_H

#include <string>
#include <type_traits>
namespace cdh{
namespace serialization{

template<typename T>
inline static
typename std::enable_if<
        std::is_same<T, std::string>::value, T>::type
strto(char const *str) {
    if (str) {
        return std::string(str);
    } else {
        return std::string();
    }
}

}
}
#endif //CDH_SERIALIZE_STD_STRING_H
