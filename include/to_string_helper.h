#ifndef MULTYTHREADING_TO_STRING_HELPER_H
#define MULTYTHREADING_TO_STRING_HELPER_H

#endif //MULTYTHREADING_TO_STRING_HELPER_H

#include <experimental/type_traits>
#include <iostream>
#include <sstream>


namespace string_caster{
    // further code is for compile-time validation
    // whereas function output
    // can be used with std::to_string
    // or with << operator
    // CHECK IF FUNCTION CAN BE USED WITH std::to_string
    template<typename to_string_castable>
    using std_to_string_expr = decltype(std::to_string(std::declval<to_string_castable>()));

    template<typename T>
    constexpr bool has_std_tostring = std::experimental::is_detected<std_to_string_expr , T>::value;
    // END CHECK IF FUNCTION CAN BE USED WITH std::to_string

    // CHECK IF FUNCTION CAN BE CAST TO STREAM
    template<typename to_stream_castable>
    using ostringstream_expression = decltype(std::declval<std::ostringstream&>() << std::declval<to_stream_castable>());

    template<typename T>
    constexpr bool has_ostringstream = std::experimental::is_detected<ostringstream_expression, T>::value;
    // END CHECK IF FUNCTION CAN BE CAST TO STREAM

    // CHECK IF FUNCTION CAN BE EITHER CAST TO STRING OR STREAM
    template <typename T>
    constexpr bool can_cast_to_string = has_ostringstream<T> && has_std_tostring<T>;
    // END CHECK IF FUNCTION CAN BE EITHER CAST TO STRING OR STREAM

    // Different implementations of templates for
    // toString function
    // both for std::string - processable
    // and << operator processable
    // 1-  std::to_string is valid on T

    template<typename T, typename std::enable_if<has_std_tostring<T>, int>::type = 0>
    std::string toString(T const& t)
    {
        return std::to_string(t);
    }

    // stream if not std::to_string castable
    template<typename T, typename std::enable_if<!has_std_tostring<T> && has_ostringstream<T>, int>::type = 0>
    std::string toString(T const& t)
    {
        std::ostringstream stream;
        stream << t;
        return stream.str();
    }

}