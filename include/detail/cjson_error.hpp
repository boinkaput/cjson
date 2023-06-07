#ifndef CJSON_ERROR_HPP
#define CJSON_ERROR_HPP

#include <exception>
#include <string>

#include "input/cjson_position.hpp"

namespace cjson::detail {
    namespace input {
        struct json_error_kind {
            virtual auto what() const noexcept -> std::string = 0;
            virtual ~json_error_kind() noexcept = default;
        };

        struct json_input_error: std::exception {
            explicit json_input_error(json_error_kind&& error_str) noexcept
                : error_str_{error_str.what()} {}

            auto what() const noexcept -> const char* override {
                return error_str_.data();
            }

            std::string error_str_;
            position pos_;
        };

        struct unterminated_string_error: json_error_kind {
            unterminated_string_error(const std::string& reason)
                : reason_{reason} {}

            auto what() const noexcept -> std::string override {
                return std::string{"Unterminated string: "} + "\"" + reason_;
            }

            std::string reason_;
        };

        struct illegal_escape_error: json_error_kind {
            illegal_escape_error(char reason)
                : reason_{reason} {}

            auto what() const noexcept -> std::string override {
                return std::string{"Illegal escape character: '\\"} + reason_ + "'";
            }

            char reason_;
        };

        struct invalid_unicode_escape_error: json_error_kind {
            invalid_unicode_escape_error(const std::string& reason)
                : reason_{reason} {}

            auto what() const noexcept -> std::string override {
                return std::string{"Invalid unicode escape character: \"\\u"} + reason_ + "\"";
            }

            std::string reason_;
        };

        struct invalid_character_error: json_error_kind {
            invalid_character_error(char reason)
                : reason_{reason} {}

            auto what() const noexcept -> std::string override {
                return std::string{"Invalid character: '"} + reason_ + "'";
            }

            char reason_;
        };

        struct invalid_minus_error: json_error_kind {
            invalid_minus_error(char reason)
                : reason_{reason} {}

            auto what() const noexcept -> std::string override {
                return std::string{"Expecting a digit after minus: got '"} + reason_ + "' instead";
            }

            char reason_;
        };

        struct invalid_fraction_error: json_error_kind {
            invalid_fraction_error(char reason)
                : reason_{reason} {}

            auto what() const noexcept -> std::string override {
                return std::string{"Expecting a digit after decimal point: got '"} + reason_ + "' instead";
            }

            char reason_;
        };

        struct invalid_exponent_error: json_error_kind {
            invalid_exponent_error(char reason, bool has_sign)
                : reason_{reason}, has_sign_{has_sign} {}

            auto what() const noexcept -> std::string override {
                if (!has_sign_) {
                    return std::string{"Expecting a digit or '+' or '-' after the exponent: got '"} + reason_ + "' instead";
                } else {
                    return std::string{"Expecting a digit after '+' or '-': got '"} + reason_ + "' instead";
                }
            }

            char reason_;
            bool has_sign_;
        };

        struct invalid_array_error: json_error_kind {
            invalid_array_error(const std::string& reason)
                : reason_{reason} {}

            auto what() const noexcept -> std::string override {
                return std::string{"Expecting ']' at the end of a json array: got \""} + reason_ + "\" instead";
            }

            std::string reason_;
        };

        struct invalid_key_error: json_error_kind {
            invalid_key_error(const std::string& reason)
                : reason_{reason} {}

            auto what() const noexcept -> std::string override {
                return std::string{"Expecting a string for key: got \""} + reason_ + "\" instead";
            }

            std::string reason_;
        };

        struct invalid_colon_error: json_error_kind {
            invalid_colon_error(const std::string& reason)
                : reason_{reason} {}

            auto what() const noexcept -> std::string override {
                return std::string{"Expecting a ':' between key-value pair: got \""} + reason_ + "\" instead";
            }

            std::string reason_;
        };

        struct invalid_object_error: json_error_kind {
            invalid_object_error(const std::string& reason)
                : reason_{reason} {}

            auto what() const noexcept -> std::string override {
                return std::string{"Expecting '}' at the end of a json object: got \""} + reason_ + "\" instead";
            }

            std::string reason_;
        };

        struct invalid_json_value_error: json_error_kind {
            invalid_json_value_error(const std::string& reason)
                : reason_{reason} {}

            auto what() const noexcept -> std::string override {
                return std::string{"Expecting json value: got \""} + reason_ + "\" instead";
            }

            std::string reason_;
        };
    }
}


#endif
