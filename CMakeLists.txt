cmake_minimum_required(VERSION 3.10)
project(cdh_serialization)

set(CMAKE_CXX_STANDARD 11)

include_directories(. /usr/include/libxml2)

set(
        cdh_serialization_SRC
        cdh/serialization/deserializer.hpp
        cdh/serialization/nvp.hpp
        cdh/serialization/serializer.hpp
        cdh/serialization/std_stl.hpp
        cdh/serialization/xml_deserializer.hpp
        cdh/serialization/xml_serializer.hpp
        cdh/serialization/processer.hpp
        cdh/serialization/singleton.hpp
        cdh/serialization/type_traits.hpp
        cdh/serialization/serialization.hpp
        cdh/serialization/std_lib.hpp
        cdh/serialization/std_string.hpp
        cdh/serialization/memory_pointer.hpp)

add_library(
        cdh_serialization
        main.cpp
        ${cdh_serialization_SRC}
)

add_subdirectory(test)
