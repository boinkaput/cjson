// struct my_types: cjson::basic_types {
//     using number_type = int;

//     template<typename Key, typename Val, typename Alloc>
//     using object_container = std::unordered_map<Key, Val, std::hash<Key>, std::equal_to<Key>, Alloc>;
// };

// using my_json_types = cjson::json_types_template<my_types>;
// using my_json = my_json_types::custom_json;

#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>
#include "cjson.hpp"
using json = cjson::basic_json<cjson::json_types>;
int main() {
    auto js0 = json{json{nullptr}, json{1.1}, json{2.1}};
    // auto js1 = cjson::json{1.1};
    // auto js2 = cjson::json{true};
    // auto js3 = cjson::json{std::string{"hello world1"}};
    // auto js4 = cjson::json{cjson::json{std::string{"hello2"}}, cjson::json{1.1}};
    // cjson::json js5 = cjson::json_types::object{{cjson::json_types::string{"hello3"}, cjson::json_types::string{"hello4"}}};
    // cjson::json js6 = js5;
    // cjson::json js7 = std::move(js5);
    // cjson::json js8 = {};
    // js8 = js7;
    // cjson::json js9 = {};
    // js9 = std::move(js7);
    // cjson::json js10 = cjson::json_types::object{
    //     {cjson::json_types::string{"hello3"}, cjson::json_types::string{"hello4"}},
    //     {cjson::json_types::string{"hello9"}, cjson::json_types::string{"hello4"}},
    //     {cjson::json_types::string{"hello13"},
    //         cjson::json_types::array{14.2, cjson::json_types::string{"hehe"}, false, {},
    //             cjson::json_types::object{{cjson::json_types::string{"hello3"}, cjson::json_types::string{"hello4"}}}
    //         }
    //     }
    // };
    // (static_cast<cjson::json_types::object&>(js10)).emplace("hello123", cjson::json_types::string{"world"});
    // (static_cast<cjson::json_types::array&>(js10[cjson::json_types::string{"hello13"}]).push_back(18283.0));
    // std::cout << js0.dump() << std::endl;
    // std::cout << js1.dump() << std::endl;
    // std::cout << js2.dump() << std::endl;
    // std::cout << js3.dump() << std::endl;
    // std::cout << js4.dump() << std::endl;
    // std::cout << js5.dump() << std::endl;
    // std::cout << js6.dump() << std::endl;
    // std::cout << js7.dump() << std::endl;
    // std::cout << js8.dump() << std::endl;
    // std::cout << js9.dump() << std::endl;
    // std::cout << js10.dump() << std::endl;
    // js10.dump(std::cout);
    // std::cout << js10 << std::endl;

    // auto j = cjson::parse<cjson::json>(
    //     R"([
    //         42, -42, 3.14159, -273.15, 2.998e8, 6.02E23, -1.602e-19,
    //         -9.10938356e-31, 3e3, 1.2E4, 3e-3, 1.2E-4, 0
    //     ])"
    // );
    // j.emplace_back<cjson::basic_types::string_type>("hello world");
    // std::cout << j << std::endl;

    // auto scanner = json_scanner<string_reader>{std::make_unique<string_reader>(
    //     R"([
    //         null, 42, -42, 3.14159, -273.15, 2.998e8, 6.02E23, -1.602e-19,
    //         -9.10938356e-31, 3e3, 1.2E4, 3e-3, 1.2E-4, 0, -1.1e
    //     ])"
    // )};
    // try {
    //     for (auto t = scanner.get_token(); t.tok_ != token::END; t = scanner.get_token()) {
    //         std::cout << "(" << t.pos_.line_start_ << ", " << t.pos_.char_start_ << ", " << t.pos_.line_end_ << ", "
    //             << t.pos_.char_end_ << "): '" << t.spelling_ << "'" << std::endl;
    //     }
    // }
    // catch (const json_scanner_error& e) {
    //     std::cerr << "(" << e.pos_.line_start_ << ", " << e.pos_.char_start_ << ", " << e.pos_.line_end_ << ", "
    //             << e.pos_.char_end_ << "): '" << e.what() << "'" << std::endl;
    // }
    // auto j = cjson::json{};
    // std::cout << std::nullptr_t{} << std::endl;

    return 0;
}
