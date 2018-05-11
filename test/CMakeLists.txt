add_executable(
        t0
        t0.cpp
)

add_executable(
        t1
        t1.cpp
)

target_link_libraries(t0 xml2 boost_serialization)
target_link_libraries(t1 xml2 boost_serialization)
