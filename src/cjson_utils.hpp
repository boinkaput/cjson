#ifndef CJSON_UTILS_HPP
#define CJSON_UTILS_HPP

#include <cstddef>
#include <string>

namespace cjson::utils {
    struct position {
        position() noexcept = default;
        position(
            const std::size_t line_start,
            const std::size_t char_start,
            const std::size_t line_end,
            const std::size_t char_end
        ) noexcept
        : line_start_{line_start}, char_start_{char_start}, line_end_{line_end}, char_end_{char_end} {}

        position(const position&) noexcept = default;
        position(position&&) noexcept = default;

        auto operator=(const position&) noexcept -> position& = default;
        auto operator=(position&&) noexcept -> position& = default;

        ~position() noexcept = default;

        auto start(const std::size_t line_start, const std::size_t char_start) noexcept -> void {
            line_start_ = line_end_ = line_start;
            char_start_ = char_end_ = char_start;
        }

        auto end(const std::size_t line_end, const std::size_t char_end) noexcept -> void {
            line_end_ = line_end;
            char_end_ = char_end;
        }

        friend auto operator==(const position& pos1, const position& pos2) noexcept -> bool = default;

        std::size_t line_start_;
        std::size_t char_start_;
        std::size_t line_end_;
        std::size_t char_end_;
    };

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
