#ifndef CJSON_POSITION_HPP
#define CJSON_POSITION_HPP

#include <cstddef>

namespace cjson::detail::input {
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
}


#endif
