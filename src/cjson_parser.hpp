#ifndef CJSON_PARSER_HPP
#define CJSON_PARSER_HPP

#include <cctype>
#include <exception>
#include <memory>

#include "cjson_reader.hpp"
#include "cjson_error.hpp"
#include "cjson_scanner.hpp"
#include "cjson_utils.hpp"

namespace cjson::parser {
    template <typename Reader, typename JsonType>
    class json_parser {
    public:
        template <typename ...Args>
        explicit json_parser(Args&& ...args) noexcept
        : scanner_{std::make_unique<Reader>(std::forward<Args>(args)...)}
        , current_token_{scanner_.get_token()} {}

        json_parser(const json_parser&) noexcept = delete;
        json_parser(json_parser&&) noexcept = default;

        auto operator=(const json_parser&) noexcept -> json_parser& = delete;
        auto operator=(json_parser&&) noexcept -> json_parser& = default;

        ~json_parser() noexcept = default;

        auto parse() -> JsonType {
            return parse_json_value();
        }

    private:
        auto accept() -> void {
            if (current_token_.tok_ != utils::token::END) {
                current_token_ = scanner_.get_token();
            }
        }

        auto parse_json_value() -> JsonType {
            if (current_token_.tok_ == utils::token::LEFT_BRACE) {
                return JsonType{parse_json_object()};
            } else if (current_token_.tok_ == utils::token::LEFT_BRACKET) {
                return JsonType{parse_json_array()};
            } else if (current_token_.tok_ == utils::token::NUMBER) {
                return JsonType{parse_json_number()};
            } else if (current_token_.tok_ == utils::token::TRUE or current_token_.tok_ == utils::token::FALSE) {
                return JsonType{parse_json_boolean()};
            } else if (current_token_.tok_ == utils::token::STRING) {
                return JsonType{parse_json_string()};
            } else if (current_token_.tok_ == utils::token::NULL_VALUE) {
                return JsonType{parse_json_null()};
            } else {
                throw error::json_error(error::invalid_json_value_error(current_token_.spelling_));
            }
        }

        auto parse_json_object() -> typename JsonType::object_t {
            auto json_object = typename JsonType::object_t{};
            accept();
            while (true) {
                if (current_token_.tok_ != utils::token::STRING) {
                    throw error::json_error(error::invalid_key_error(current_token_.spelling_));
                }
                const auto key = parse_json_string();
                if (current_token_.tok_ != utils::token::COLON) {
                    throw error::json_error(error::invalid_colon_error(current_token_.spelling_));
                }
                accept();
                json_object.emplace(key, parse_json_value());
                if (current_token_.tok_ == utils::token::RIGHT_BRACE) {
                    accept();
                    break;
                } else if (current_token_.tok_ != utils::token::COMMA) {
                    throw error::json_error(error::invalid_object_error(current_token_.spelling_));
                }
                accept();
            }
            return json_object;
        }

        auto parse_json_array() -> typename JsonType::array_t {
            auto json_array = typename JsonType::array_t{};
            accept();
            while (true) {
                json_array.emplace_back(parse_json_value());
                if (current_token_.tok_ == utils::token::RIGHT_BRACKET) {
                    accept();
                    break;
                } else if (current_token_.tok_ != utils::token::COMMA) {
                    throw error::json_error(error::invalid_array_error(current_token_.spelling_));
                }
                accept();
            }
            return json_array;
        }

        auto parse_json_number() -> typename JsonType::number_t {
            const auto& json_number = typename JsonType::number_t{std::stod(current_token_.spelling_)};
            accept();
            return json_number;
        }

        auto parse_json_boolean() -> typename JsonType::boolean_t {
            const auto& json_boolean = typename JsonType::boolean_t{current_token_.spelling_ == _TRUE_TOK};
            accept();
            return json_boolean;
        }

        auto parse_json_string() -> typename JsonType::string_t {
            const auto& json_string = typename JsonType::string_t{current_token_.spelling_};
            accept();
            return json_string;
        }

        auto parse_json_null() -> typename JsonType::null_t {
            accept();
            return typename JsonType::null_t{};
        }

        scanner::json_scanner<Reader> scanner_;
        utils::json_token current_token_;
    };
}


#endif
