//
// Created by chudonghao on 18-5-22.
//

#ifndef CDH_SERIALIZATION_STD_LIB_H
#define CDH_SERIALIZATION_STD_LIB_H

#include <cstdlib>
#include <type_traits>

namespace cdh {
namespace serialization {

template<typename T>
inline static
typename std::enable_if<
        std::is_same<T, char>::value
        || std::is_same<T, int>::value
        || std::is_same<T, long>::value, T>::type
strto(char const *str) {
    if (str) {
        return std::strtol(str, nullptr, 10);
    } else {
        return static_cast<T>(0);
    }
}

template<typename T>
inline static
typename std::enable_if<
        std::is_same<T, long long>::value
        || std::is_same<T, long long int>::value, T>::type
strto(char const *str) {
    if (str) {
        return std::strtoll(str, nullptr, 10);
    } else {
        return static_cast<T>(0);
    }
}

template<typename T>
inline static
typename std::enable_if<
        std::is_same<T, unsigned char>::value
        || std::is_same<T, unsigned int>::value
        || std::is_same<T, unsigned long>::value, T>::type
strto(char const *str) {
    if (str) {
        return std::strtoul(str, nullptr, 10);
    } else {
        return static_cast<T>(0);
    }
}

template<typename T>
inline static
typename std::enable_if<
        std::is_same<T, unsigned long long>::value
        || std::is_same<T, unsigned long long int>::value, T>::type
strto(char const *str) {
    if (str) {
        return std::strtoull(str, nullptr, 10);
    } else {
        return static_cast<T>(0);
    }
}

template<typename T>
inline static
typename std::enable_if<
        std::is_same<T, float>::value, T>::type
strto(char const *str) {
    if (str) {
        return std::strtof(str, nullptr);
    } else {
        return static_cast<T>(0);
    }
}

template<typename T>
inline static
typename std::enable_if<
        std::is_same<T, double>::value, T>::type
strto(char const *str) {
    if (str) {
        return std::strtod(str, nullptr);
    } else {
        return static_cast<T>(0);
    }
}

template<typename T>
inline static
typename std::enable_if<
        std::is_same<T, long double>::value, T>::type
strto(char const *str) {
    if (str) {
        return std::strtold(str, nullptr);
    } else {
        return static_cast<T>(0);
    }
}

}
}
#endif //CDH_SERIALIZE_STD_LIB_H
