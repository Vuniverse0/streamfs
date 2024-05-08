//
// Created by vuniverse on 5/4/24.
//

#ifndef STREAMFS_HASMETHOD_HPP
#define STREAMFS_HASMETHOD_HPP

#include <utility>

#define SeekMethod(MethodName)       \
template< class , class = void >     \
struct Has_##MethodName##Method      \
: std::false_type{   };              \
template<typename T>                 \
struct Has_##MethodName##Method      \
<T, std::void_t<decltype(std::declval<T>().MethodName())>> \
: std::true_type{     };                                   \

#endif //STREAMFS_HASMETHOD_HPP