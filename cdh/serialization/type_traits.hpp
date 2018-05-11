//
// Created by chudonghao on 18-5-20.
//

#ifndef CDH_SERIALIZATION_TYPE_TRAITS_H
#define CDH_SERIALIZATION_TYPE_TRAITS_H

#include <type_traits>
#include <string>

namespace cdh {
namespace serialization {
    /**
     * 判断T是不是arithmetic或std::string
     * @tparam T
     * @retval value:true || false
     */
    template<typename T>
    class is_basic_type : public std::integral_constant<bool,
            std::is_same<std::string, T>::value || std::is_arithmetic<T>::value> {
    };

    /**
     * 判断T是不是用户自定义的类
     * @tparam T
     * @retval value:true || false
     */
    template<typename T>
    class is_user_class : public std::integral_constant<bool,
            !is_basic_type<T>::value && std::is_class<T>::value && !std::is_polymorphic<T>::value> {
    };


    /**
     * 判断T是不是用户自定义的支持rtti的类
     * @tparam T
     * @retval value:true || false
     */
    template<typename T>
    class is_user_rtti_class : public std::integral_constant<bool,
            !is_basic_type<T>::value && std::is_class<T>::value && std::is_polymorphic<T>::value> {
    };

    /**
     * 判断T是不是用户自定义的类的指针
     * @tparam T
     * @tparam depth 指针深度
     * @retval value:true || false
     */
    template<typename T, int level>
    class is_user_pointer;

    template<typename T>
    class is_user_pointer<T, 1> : public std::integral_constant<bool, std::is_pointer<T>::value &&
                                                                      (is_user_class<typename std::remove_pointer<T>::type>::value ||
                                                                       is_user_rtti_class<typename std::remove_pointer<T>::type>::value)> {
    };

    template<typename T>
    class is_unsupport : public std::integral_constant<bool,
            !is_basic_type<T>::value && !is_user_class<T>::value && !is_user_rtti_class<T>::value &&
            !is_user_pointer<T, 1>::value> {
    };

}
}
#endif //CDH_SERIALIZATION_TYPE_TRAITS_H
