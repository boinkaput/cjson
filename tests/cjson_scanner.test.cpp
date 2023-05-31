#include <catch2/catch.hpp>
#include <memory>
#include <string_view>

#include "../src/cjson_scanner.hpp"

template <typename Reader, typename... Args>
auto make_scanner(Args &&...str) -> cjson::scanner::json_scanner<Reader> {
    return cjson::scanner::json_scanner<Reader>{
        std::make_unique<Reader>(std::forward<Args>(str)...)};
}

TEST_CASE("Empty input string") {
    auto scanner = make_scanner<cjson::reader::string_reader>("");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 0, 0, 0}});

    scanner = make_scanner<cjson::reader::string_reader>(R"(

    )");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{2, 5, 2, 5}});
}

TEST_CASE("Json null") {
    auto scanner = make_scanner<cjson::reader::string_reader>("null");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NULL_VALUE, "null",
                                     cjson::utils::position{0, 0, 0, 4}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 4, 0, 4}});

    scanner = make_scanner<cjson::reader::string_reader>(R"(
        null
    )");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NULL_VALUE, "null",
                                     cjson::utils::position{1, 9, 1, 13}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{2, 5, 2, 5}});
}

TEST_CASE("Json boolean") {
    auto scanner = make_scanner<cjson::reader::string_reader>("true");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::TRUE, "true",
                                     cjson::utils::position{0, 0, 0, 4}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 4, 0, 4}});

    scanner = make_scanner<cjson::reader::string_reader>("false");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::FALSE, "false",
                                     cjson::utils::position{0, 0, 0, 5}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 5, 0, 5}});

    scanner = make_scanner<cjson::reader::string_reader>(R"(
        true
        false
    )");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::TRUE, "true",
                                     cjson::utils::position{1, 9, 1, 13}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::FALSE, "false",
                                     cjson::utils::position{2, 9, 2, 14}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{3, 5, 3, 5}});
}

TEST_CASE("Json number") {
    auto scanner = make_scanner<cjson::reader::string_reader>("0");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "0",
                                     cjson::utils::position{0, 0, 0, 1}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 1, 0, 1}});

    scanner = make_scanner<cjson::reader::string_reader>("123");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "123",
                                     cjson::utils::position{0, 0, 0, 3}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 3, 0, 3}});

    scanner = make_scanner<cjson::reader::string_reader>("-123.456");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "-123.456",
                                     cjson::utils::position{0, 0, 0, 8}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 8, 0, 8}});

    scanner = make_scanner<cjson::reader::string_reader>("0.5e+3");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "0.5e+3",
                                     cjson::utils::position{0, 0, 0, 6}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 6, 0, 6}});

    scanner = make_scanner<cjson::reader::string_reader>("-0.123e+4");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "-0.123e+4",
                                     cjson::utils::position{0, 0, 0, 9}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 9, 0, 9}});

    scanner = make_scanner<cjson::reader::string_reader>("2e10");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "2e10",
                                     cjson::utils::position{0, 0, 0, 4}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 4, 0, 4}});

    scanner = make_scanner<cjson::reader::string_reader>("-2e-10");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "-2e-10",
                                     cjson::utils::position{0, 0, 0, 6}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 6, 0, 6}});

    scanner = make_scanner<cjson::reader::string_reader>(R"(
        123
        -456.789
        0.123e-4
        5.15e9
    )");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "123",
                                     cjson::utils::position{1, 9, 1, 12}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "-456.789",
                                     cjson::utils::position{2, 9, 2, 17}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "0.123e-4",
                                     cjson::utils::position{3, 9, 3, 17}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "5.15e9",
                                     cjson::utils::position{4, 9, 4, 15}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{5, 5, 5, 5}});
}

TEST_CASE("Json string") {
    auto scanner = make_scanner<cjson::reader::string_reader>(R"("")");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "",
                                     cjson::utils::position{0, 0, 0, 2}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 2, 0, 2}});

    scanner = make_scanner<cjson::reader::string_reader>(R"("hello world")");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "hello world",
                                     cjson::utils::position{0, 0, 0, 13}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 13, 0, 13}});

    scanner = make_scanner<cjson::reader::string_reader>(R"(
        "hello world"
    )");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "hello world",
                                     cjson::utils::position{1, 9, 1, 22}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{2, 5, 2, 5}});

    scanner =
        make_scanner<cjson::reader::string_reader>(R"("hello \"world\"")");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING,
                                     "hello \"world\"",
                                     cjson::utils::position{0, 0, 0, 17}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 17, 0, 17}});

    scanner = make_scanner<cjson::reader::string_reader>(R"("hello\nworld")");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING,
                                     "hello\nworld",
                                     cjson::utils::position{0, 0, 0, 14}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 14, 0, 14}});

    scanner = make_scanner<cjson::reader::string_reader>(R"("\t\n\r\b\f\"\\")");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING,
                                     "\t\n\r\b\f\"\\",
                                     cjson::utils::position{0, 0, 0, 16}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 16, 0, 16}});

    scanner = make_scanner<cjson::reader::string_reader>(R"(
        "Hello, \tWorld\nHow are you? \r\bThis is awesome \fBye! \"\u263a\""
    )");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{
                cjson::utils::token::STRING,
                "Hello, \tWorld\nHow are you? \r\bThis is awesome \fBye! \":\"",
                cjson::utils::position{1, 9, 1, 77}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{2, 5, 2, 5}});
}

TEST_CASE("Empty JSON array") {
    auto scanner = make_scanner<cjson::reader::string_reader>("[]");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{0, 0, 0, 1}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{0, 1, 0, 2}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 2, 0, 2}});

    scanner = make_scanner<cjson::reader::string_reader>(R"(
        [
        ]
    )");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{1, 9, 1, 10}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{2, 9, 2, 10}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{3, 5, 3, 5}});
}

TEST_CASE("JSON array with one element") {
    auto scanner = make_scanner<cjson::reader::string_reader>("[null]");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{0, 0, 0, 1}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NULL_VALUE, "null",
                                     cjson::utils::position{0, 1, 0, 5}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{0, 5, 0, 6}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 6, 0, 6}});

    scanner = make_scanner<cjson::reader::string_reader>(R"(
        [
            null
        ]
    )");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{1, 9, 1, 10}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NULL_VALUE, "null",
                                     cjson::utils::position{2, 13, 2, 17}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{3, 9, 3, 10}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{4, 5, 4, 5}});
}

TEST_CASE("JSON Array of Numbers") {
    auto scanner =
        make_scanner<cjson::reader::string_reader>("[1, 2, 3.14, -4, 0]");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{0, 0, 0, 1}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "1",
                                     cjson::utils::position{0, 1, 0, 2}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 2, 0, 3}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "2",
                                     cjson::utils::position{0, 4, 0, 5}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 5, 0, 6}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "3.14",
                                     cjson::utils::position{0, 7, 0, 11}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 11, 0, 12}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "-4",
                                     cjson::utils::position{0, 13, 0, 15}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 15, 0, 16}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "0",
                                     cjson::utils::position{0, 17, 0, 18}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{0, 18, 0, 19}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 19, 0, 19}});
}

TEST_CASE("JSON Array of Booleans") {
    auto scanner = make_scanner<cjson::reader::string_reader>("[true, false]");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{0, 0, 0, 1}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::TRUE, "true",
                                     cjson::utils::position{0, 1, 0, 5}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 5, 0, 6}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::FALSE, "false",
                                     cjson::utils::position{0, 7, 0, 12}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{0, 12, 0, 13}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 13, 0, 13}});
}

TEST_CASE("JSON Array of Strings") {
    auto scanner = make_scanner<cjson::reader::string_reader>(R"([
        "hello",
        "world",
        "goodbye"
    ])");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{0, 0, 0, 1}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "hello",
                                     cjson::utils::position{1, 9, 1, 16}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{1, 16, 1, 17}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "world",
                                     cjson::utils::position{2, 9, 2, 16}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{2, 16, 2, 17}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "goodbye",
                                     cjson::utils::position{3, 9, 3, 18}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{4, 5, 4, 6}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{4, 6, 4, 6}});
}

TEST_CASE("Json array with nested arrays and objects") {
    auto scanner = make_scanner<cjson::reader::string_reader>(
        "[[1, 2], {\"foo\": \"bar\"}, [3, 4], {\"baz\": 5}]");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{0, 0, 0, 1}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{0, 1, 0, 2}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "1",
                                     cjson::utils::position{0, 2, 0, 3}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 3, 0, 4}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "2",
                                     cjson::utils::position{0, 5, 0, 6}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{0, 6, 0, 7}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 7, 0, 8}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACE, "{",
                                     cjson::utils::position{0, 9, 0, 10}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "foo",
                                     cjson::utils::position{0, 10, 0, 15}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{0, 15, 0, 16}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "bar",
                                     cjson::utils::position{0, 17, 0, 22}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACE, "}",
                                     cjson::utils::position{0, 22, 0, 23}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 23, 0, 24}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{0, 25, 0, 26}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "3",
                                     cjson::utils::position{0, 26, 0, 27}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 27, 0, 28}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "4",
                                     cjson::utils::position{0, 29, 0, 30}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{0, 30, 0, 31}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 31, 0, 32}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACE, "{",
                                     cjson::utils::position{0, 33, 0, 34}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "baz",
                                     cjson::utils::position{0, 34, 0, 39}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{0, 39, 0, 40}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "5",
                                     cjson::utils::position{0, 41, 0, 42}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACE, "}",
                                     cjson::utils::position{0, 42, 0, 43}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{0, 43, 0, 44}});
}

TEST_CASE("Empty JSON object") {
    auto scanner = make_scanner<cjson::reader::string_reader>("{}");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACE, "{",
                                     cjson::utils::position{0, 0, 0, 1}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACE, "}",
                                     cjson::utils::position{0, 1, 0, 2}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 2, 0, 2}});
}

TEST_CASE("JSON object with null value") {
    auto scanner =
        make_scanner<cjson::reader::string_reader>("{\"key\": null}");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACE, "{",
                                     cjson::utils::position{0, 0, 0, 1}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "key",
                                     cjson::utils::position{0, 1, 0, 6}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{0, 6, 0, 7}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NULL_VALUE, "null",
                                     cjson::utils::position{0, 8, 0, 12}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACE, "}",
                                     cjson::utils::position{0, 12, 0, 13}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 13, 0, 13}});
}

TEST_CASE("JSON object with one pair") {
    auto scanner = make_scanner<cjson::reader::string_reader>("{\"key\": 123}");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACE, "{",
                                     cjson::utils::position{0, 0, 0, 1}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "key",
                                     cjson::utils::position{0, 1, 0, 6}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{0, 6, 0, 7}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "123",
                                     cjson::utils::position{0, 8, 0, 11}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACE, "}",
                                     cjson::utils::position{0, 11, 0, 12}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 12, 0, 12}});
}

TEST_CASE("JSON object with multiple pairs") {
    auto scanner = make_scanner<cjson::reader::string_reader>(
        "{\"key1\": true, \"key2\": [1, 2, 3], \"key3\": {\"nested\": "
        "\"value\"}}");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACE, "{",
                                     cjson::utils::position{0, 0, 0, 1}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "key1",
                                     cjson::utils::position{0, 1, 0, 7}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{0, 7, 0, 8}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::TRUE, "true",
                                     cjson::utils::position{0, 9, 0, 13}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 13, 0, 14}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "key2",
                                     cjson::utils::position{0, 15, 0, 21}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{0, 21, 0, 22}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{0, 23, 0, 24}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "1",
                                     cjson::utils::position{0, 24, 0, 25}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 25, 0, 26}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "2",
                                     cjson::utils::position{0, 27, 0, 28}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 28, 0, 29}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "3",
                                     cjson::utils::position{0, 30, 0, 31}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{0, 31, 0, 32}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{0, 32, 0, 33}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "key3",
                                     cjson::utils::position{0, 34, 0, 40}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{0, 40, 0, 41}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACE, "{",
                                     cjson::utils::position{0, 42, 0, 43}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "nested",
                                     cjson::utils::position{0, 43, 0, 51}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{0, 51, 0, 52}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "value",
                                     cjson::utils::position{0, 53, 0, 60}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACE, "}",
                                     cjson::utils::position{0, 60, 0, 61}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACE, "}",
                                     cjson::utils::position{0, 61, 0, 62}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{0, 62, 0, 62}});
}

TEST_CASE("JSON object with nested arrays and objects") {
    auto scanner = make_scanner<cjson::reader::string_reader>(R"(
        {
            "name": "John Doe",
            "age": 20,
            "isStudent": true,
            "address": {
                "street": "123 Main St",
                "city": "Anytown",
                "state": "CA",
                "zip": "12345"
            },
            "phoneNumbers": [
                "555-1234",
                "555-5678"
            ]
        }
    )");
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACE, "{",
                                     cjson::utils::position{1, 9, 1, 10}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "name",
                                     cjson::utils::position{2, 13, 2, 19}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{2, 19, 2, 20}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "John Doe",
                                     cjson::utils::position{2, 21, 2, 31}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{2, 31, 2, 32}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "age",
                                     cjson::utils::position{3, 13, 3, 18}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{3, 18, 3, 19}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::NUMBER, "20",
                                     cjson::utils::position{3, 20, 3, 22}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{3, 22, 3, 23}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "isStudent",
                                     cjson::utils::position{4, 13, 4, 24}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{4, 24, 4, 25}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::TRUE, "true",
                                     cjson::utils::position{4, 26, 4, 30}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{4, 30, 4, 31}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "address",
                                     cjson::utils::position{5, 13, 5, 22}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{5, 22, 5, 23}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACE, "{",
                                     cjson::utils::position{5, 24, 5, 25}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "street",
                                     cjson::utils::position{6, 17, 6, 25}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{6, 25, 6, 26}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "123 Main St",
                                     cjson::utils::position{6, 27, 6, 40}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{6, 40, 6, 41}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "city",
                                     cjson::utils::position{7, 17, 7, 23}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{7, 23, 7, 24}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "Anytown",
                                     cjson::utils::position{7, 25, 7, 34}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{7, 34, 7, 35}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "state",
                                     cjson::utils::position{8, 17, 8, 24}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{8, 24, 8, 25}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "CA",
                                     cjson::utils::position{8, 26, 8, 30}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{8, 30, 8, 31}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "zip",
                                     cjson::utils::position{9, 17, 9, 22}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{9, 22, 9, 23}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "12345",
                                     cjson::utils::position{9, 24, 9, 31}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACE, "}",
                                     cjson::utils::position{10, 13, 10, 14}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{10, 14, 10, 15}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING,
                                     "phoneNumbers",
                                     cjson::utils::position{11, 13, 11, 27}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COLON, ":",
                                     cjson::utils::position{11, 27, 11, 28}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::LEFT_BRACKET, "[",
                                     cjson::utils::position{11, 29, 11, 30}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "555-1234",
                                     cjson::utils::position{12, 17, 12, 27}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::COMMA, ",",
                                     cjson::utils::position{12, 27, 12, 28}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::STRING, "555-5678",
                                     cjson::utils::position{13, 17, 13, 27}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACKET, "]",
                                     cjson::utils::position{14, 13, 14, 14}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::RIGHT_BRACE, "}",
                                     cjson::utils::position{15, 9, 15, 10}});
    REQUIRE(scanner.get_token() ==
            cjson::utils::json_token{cjson::utils::token::END, "",
                                     cjson::utils::position{16, 5, 16, 5}});
}

TEST_CASE("Json Errors") {
    auto scanner = make_scanner<cjson::reader::string_reader>("\"hello world");
    REQUIRE_THROWS_MATCHES(
        scanner.get_token(), cjson::error::json_error,
        Catch::Message("Unterminated string: \"hello world"));

    scanner = make_scanner<cjson::reader::string_reader>("\"hello \\z world\"");
    REQUIRE_THROWS_MATCHES(scanner.get_token(), cjson::error::json_error,
                           Catch::Message("Illegal escape character: '\\z'"));

    scanner =
        make_scanner<cjson::reader::string_reader>("\"hello \\uXYZA world\"");
    REQUIRE_THROWS_MATCHES(
        scanner.get_token(), cjson::error::json_error,
        Catch::Message("Invalid unicode escape character: \"\\uXYZA\""));

    scanner = make_scanner<cjson::reader::string_reader>("hello world");
    REQUIRE_THROWS_MATCHES(scanner.get_token(), cjson::error::json_error,
                           Catch::Message("Invalid character: 'h'"));

    scanner = make_scanner<cjson::reader::string_reader>("-abc");
    REQUIRE_THROWS_MATCHES(
        scanner.get_token(), cjson::error::json_error,
        Catch::Message("Expecting a digit after minus: got 'a' instead"));

    scanner = make_scanner<cjson::reader::string_reader>("3.abc");
    REQUIRE_THROWS_MATCHES(
        scanner.get_token(), cjson::error::json_error,
        Catch::Message(
            "Expecting a digit after decimal point: got 'a' instead"));

    scanner = make_scanner<cjson::reader::string_reader>("3eabc");
    REQUIRE_THROWS_MATCHES(
        scanner.get_token(), cjson::error::json_error,
        Catch::Message("Expecting a digit or '+' or '-' after "
                       "the exponent: got 'a' instead"));

    scanner = make_scanner<cjson::reader::string_reader>("3e+abc");
    REQUIRE_THROWS_MATCHES(
        scanner.get_token(), cjson::error::json_error,
        Catch::Message("Expecting a digit after '+' or '-': got 'a' instead"));

    scanner = make_scanner<cjson::reader::string_reader>(
        "\"hello\\zhee haw\"\"\\nworld");
    REQUIRE_THROWS_MATCHES(scanner.get_token(), cjson::error::json_error,
                           Catch::Message("Illegal escape character: '\\z'"));
    REQUIRE_THROWS_MATCHES(scanner.get_token(), cjson::error::json_error,
                           Catch::Message("Unterminated string: \"\nworld"));
}
