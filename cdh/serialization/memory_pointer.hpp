//
// Created by chudonghao on 18-5-22.
//

#ifndef CDH_SERIALIZATION_MEMORY_POINTER_H
#define CDH_SERIALIZATION_MEMORY_POINTER_H

#include <cstring>

namespace cdh {
namespace serialization {

class memory_pointer {
public:
    memory_pointer(char const *name, void const *ptr) : name(name), ptr(ptr) {}

    char const *name;
    void const *ptr;

};

bool operator<(const memory_pointer &left, const memory_pointer &right) {
    if (left.ptr < right.ptr) {
        //ptr < ptr
        return true;
    } else if (left.ptr == right.ptr) {
        int i = strcmp(left.name, right.name);
        if (i < 0) {
            // ptr == ptr && name < name
            return true;
        } else {
            // ptr == ptr && name == name
            return false;
        }
    } else {
        // ptr > ptr
        return false;
    }
}

}
}

#endif //CDH_SERIALIZATION_MEMORY_POINTER_H
