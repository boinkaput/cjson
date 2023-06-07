#ifndef CJSON_FWD_HPP
#define CJSON_FWD_HPP

#include "cjson_basic.hpp"

namespace cjson {
    struct json_types {
        using number_type = double;
        using boolean_type = bool;
        using string_type = std::string;

        template <typename Key, typename Val, typename Alloc>
        using object_type = std::map<Key, Val, std::less<string_type>, Alloc>;

        template <typename Val, typename Alloc>
        using array_type = std::vector<Val, Alloc>;
    };

    using json = basic_json<json_types>;
}


#endif
