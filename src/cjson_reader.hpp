#ifndef CJSON_READER_HPP
#define CJSON_READER_HPP

#include <string_view>

#define CHAR_END '\0'

namespace cjson::reader {
    class json_reader {
    public:
        json_reader() noexcept = default;
        json_reader(const json_reader&) noexcept = delete;
        json_reader(json_reader&&) noexcept = default;

        auto operator=(const json_reader&) noexcept -> json_reader& = delete;
        auto operator=(json_reader&&) noexcept -> json_reader& = default;

        virtual ~json_reader() noexcept = default;

        virtual auto advance() noexcept -> char = 0;
    };

    class string_reader: json_reader {
    public:
        string_reader() noexcept = default;
        string_reader(std::string_view&& str) noexcept
        : str_{str}, cur_{str.begin()} {}

        ~string_reader() noexcept override = default;

        auto advance() noexcept -> char override {
            if (cur_ == str_.end()) {
                return CHAR_END;
            } else {
                return *(cur_++);
            }
        }

    private:
        std::string_view str_;
        std::string_view::const_iterator cur_;
    };
}


#endif
