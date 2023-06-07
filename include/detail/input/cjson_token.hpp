#ifndef CJSON_TOKEN_HPP
#define CJSON_TOKEN_HPP

#include <string>

#include "cjson_position.hpp"

namespace cjson::detail::input {
    enum class token: unsigned short int {
        END,
        LEFT_BRACE,
        RIGHT_BRACE,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        COMMA,
        COLON,
        NULL_VALUE,
        TRUE,
        FALSE,
        NUMBER,
        STRING
    };

    struct json_token {
        json_token() noexcept = default;
        json_token(token tok, std::string&& spelling, position&& pos)
            : tok_{tok}, spelling_{std::move(spelling)}, pos_{std::move(pos)} {}

        json_token(const json_token&) noexcept = default;
        json_token(json_token&&) noexcept = default;

        auto operator=(const json_token&) noexcept -> json_token& = default;
        auto operator=(json_token&&) noexcept -> json_token& = default;

        ~json_token() noexcept = default;

        friend auto operator==(const json_token& pos1, const json_token& pos2) noexcept -> bool = default;

        token tok_;
        std::string spelling_;
        position pos_;
    };
}


#endif
