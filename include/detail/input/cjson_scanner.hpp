#ifndef CJSON_SCANNER_HPP
#define CJSON_SCANNER_HPP

#include <cctype>
#include <memory>
#include <string>
#include <unordered_set>

#include "../cjson_error.hpp"
#include "cjson_reader.hpp"
#include "cjson_token.hpp"

#define _NULL_TERMINATOR '\0'
#define _NEWLINE '\n'
#define _CARRIAGERETURN '\r'
#define _TAB '\t'
#define _BACKSPACE '\b'
#define _FORMFEED '\f'
#define _QUOTE '\"'
#define _FORWARDSLASH '/'
#define _BACKSLASH '\\'
#define _DEFAULT_TAB_WIDTH 4
#define _ZERO '0'
#define _DECIMAL_POINT '.'
#define _EXPONENT 'e'
#define _PLUS '+'
#define _MINUS '-'
#define _LEFT_BRACE_TOK '{'
#define _RIGHT_BRACE_TOK '}'
#define _LEFT_BRACKET_TOK '['
#define _RIGHT_BRACKET_TOK ']'
#define _COMMA_TOK ','
#define _COLON_TOK ':'
#define _NULL_TOK "null"
#define _TRUE_TOK "true"
#define _FALSE_TOK "false"

namespace cjson::detail::input {
    template <typename Reader>
    class json_scanner {
    public:
        explicit json_scanner(
            std::unique_ptr<Reader>&& reader,
            const std::size_t line_number = 0,
            const std::size_t char_number = 0,
            const std::size_t tab_width = _DEFAULT_TAB_WIDTH
        ) noexcept
            : reader_{std::move(reader)}
            , current_char_{reader_->advance()}
            , line_number_{line_number}
            , char_number_{char_number}
            , tab_width_{tab_width} {}

        json_scanner(const json_scanner&) noexcept = delete;
        json_scanner(json_scanner&&) noexcept = default;

        auto operator=(const json_scanner&) noexcept -> json_scanner& = delete;
        auto operator=(json_scanner&&) noexcept -> json_scanner& = default;

        ~json_scanner() noexcept = default;

        auto get_token() -> json_token {
            auto spelling = std::string{};
            auto pos = position{};
            auto tok = token{};
            skip_spaces();

            pos.start(line_number_, char_number_);
            try {
                tok = next_token(spelling, tok);
                pos.end(line_number_, char_number_);
                return json_token(tok, std::move(spelling), std::move(pos));
            } catch (json_input_error& error) {
                pos.end(line_number_, char_number_);
                error.pos_ = std::move(pos);
                if (tok == token::STRING) {
                    while (current_char_ != _NULL_TERMINATOR and current_char_ != _QUOTE) {
                        accept();
                    }
                    accept();
                }
                throw error;
            }
        }

    private:
        inline static const std::unordered_set<char> hex_digits = {
            '0', '1', '2', '3', '4', '5', '6',
            '7', '8', '9', 'a', 'b', 'c', 'd',
            'e', 'f', 'A', 'B', 'C', 'D', 'E', 'F'
        };

        auto accept() noexcept -> void {
            if (current_char_ == _NEWLINE) {
                ++line_number_;
                char_number_ = 0;
            } else if (current_char_ == _TAB) {
                char_number_ += (tab_width_ + 1) - (char_number_ % tab_width_);
            }
            current_char_ = reader_->advance();
            ++char_number_;
        }

        auto skip_spaces() noexcept -> void {
            while (std::isspace(current_char_)) {
                accept();
            }
        }

        auto next_token(std::string& spelling, token& tok) -> token {
            switch (current_char_) {
                case _NULL_TERMINATOR:
                    break;
                case _LEFT_BRACE_TOK:
                    tok = token::LEFT_BRACE;
                    spelling.append(1, _LEFT_BRACE_TOK);
                    accept();
                    break;
                case _RIGHT_BRACE_TOK:
                    tok = token::RIGHT_BRACE;
                    spelling.append(1, _RIGHT_BRACE_TOK);
                    accept();
                    break;
                case _LEFT_BRACKET_TOK:
                    tok = token::LEFT_BRACKET;
                    spelling.append(1, _LEFT_BRACKET_TOK);
                    accept();
                    break;
                case _RIGHT_BRACKET_TOK:
                    tok = token::RIGHT_BRACKET;
                    spelling.append(1, _RIGHT_BRACKET_TOK);
                    accept();
                    break;
                case _COMMA_TOK:
                    tok = token::COMMA;
                    spelling.append(1, _COMMA_TOK);
                    accept();
                    break;
                case _COLON_TOK:
                    tok = token::COLON;
                    spelling.append(1, _COLON_TOK);
                    accept();
                    break;
                case _QUOTE:
                    accept();
                    tok = token::STRING;
                    scan_string(spelling);
                    accept();
                    break;
                default:
                    if (std::isalpha(current_char_)) {
                        scan_keywords(spelling);
                        if (spelling == _TRUE_TOK) {
                            tok = token::TRUE;
                        } else if (spelling == _FALSE_TOK) {
                            tok = token::FALSE;
                        } else if (spelling == _NULL_TOK) {
                            tok = token::NULL_VALUE;
                        }
                    } else {
                        scan_number(spelling);
                        if (not spelling.empty()) {
                            tok = token::NUMBER;
                        }
                    }

                    if (tok == token::END) {
                        throw json_input_error(invalid_character_error(spelling.front()));
                    }
                    break;
            };
            return tok;
        }

        auto scan_string(std::string& spelling) -> void {
            while (current_char_ != _QUOTE) {
                if (current_char_ == _NULL_TERMINATOR) {
                    throw json_input_error(unterminated_string_error(spelling));
                } else if (current_char_ == _BACKSLASH) {
                    accept();
                    if (current_char_ == 'b') {
                        spelling.append(1, _BACKSPACE);
                    } else if (current_char_ == 'f') {
                        spelling.append(1, _FORMFEED);
                    } else if (current_char_ == 'n') {
                        spelling.append(1, _NEWLINE);
                    } else if (current_char_ == 'r') {
                        spelling.append(1, _CARRIAGERETURN);
                    } else if (current_char_ == 't') {
                        spelling.append(1, _TAB);
                    } else if (current_char_ == 'u') {
                        auto error = false;
                        auto hex_str = std::string{};
                        for (int i = 0; i < 4; ++i) {
                            accept();
                            hex_str.append(1, current_char_);
                            if (!hex_digits.contains(current_char_)) {
                                error = true;
                            }
                        }
                        if (error) {
                            throw json_input_error(invalid_unicode_escape_error(hex_str));
                        }
                        spelling.append(1, static_cast<char>(std::stoi(hex_str, nullptr, 16)));
                    } else if (current_char_ == '"') {
                        spelling.append(1, _QUOTE);
                    } else if (current_char_ == '/') {
                        spelling.append(1, _FORWARDSLASH);
                    } else if (current_char_ == '\\') {
                        spelling.append(1, _BACKSLASH);
                    } else {
                        throw json_input_error(illegal_escape_error(current_char_));
                    }
                } else {
                    spelling.append(1, current_char_);
                }
                accept();
            }
        }

        auto scan_number(std::string& spelling) -> void {
            if (current_char_ == _MINUS) {
                spelling.append(1, _MINUS);
                accept();
                if (not std::isdigit(current_char_)) {
                    throw json_input_error(invalid_minus_error(current_char_));
                }
            }

            if (current_char_ == _ZERO) {
                spelling.append(1, _ZERO);
                accept();
            } else {
                scan_digits(spelling);
            }

            if (not spelling.empty()) {
                if (current_char_ == _DECIMAL_POINT) {
                    spelling.append(1, _DECIMAL_POINT);
                    accept();
                    if (not std::isdigit(current_char_)) {
                        throw json_input_error(invalid_fraction_error(current_char_));
                    }
                    scan_digits(spelling);
                }

                if (std::tolower(current_char_) == _EXPONENT) {
                    spelling.append(1, current_char_);
                    accept();
                    const char prev_char = current_char_;
                    accept();
                    if (not (std::isdigit(prev_char) or ((prev_char == _PLUS or prev_char == _MINUS)
                        and std::isdigit(current_char_)))) {
                        if ((prev_char == _PLUS or prev_char == _MINUS)) {
                            throw json_input_error(invalid_exponent_error(current_char_, true));
                        } else {
                            throw json_input_error(invalid_exponent_error(prev_char, false));
                        }
                    }
                    spelling.append(1, prev_char);
                    scan_digits(spelling);
                }
            }
        }

        auto scan_digits(std::string& spelling) -> void {
            while (std::isdigit(current_char_)) {
                spelling.append(1, current_char_);
                accept();
            }
        }

        auto scan_keywords(std::string& spelling) -> void {
            while (std::isalpha(current_char_)) {
                spelling.append(1, current_char_);
                accept();
            }
        }

        std::unique_ptr<Reader> reader_;
        char current_char_;
        std::size_t line_number_;
        std::size_t char_number_;
        std::size_t tab_width_;
    };
}


#endif
