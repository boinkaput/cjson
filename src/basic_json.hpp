#ifndef JSON_HPP
#define JSON_HPP

#include <cstddef>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace cjson {
    struct types {
        using number_type = double;
        using boolean_type = bool;
        using string_type = std::string;

        template <typename Key, typename Val, typename Alloc>
        using object_type = std::map<Key, Val, std::less<string_type>, Alloc>;

        template <typename Val, typename Alloc>
        using array_type = std::vector<Val, Alloc>;
    };

    template <typename Types = types, template<typename> class Alloc = std::allocator>
    class basic_json {
    public:
        using null = std::nullptr_t;
        using number = typename Types::number_type;
        using boolean = typename Types::boolean_type;
        using string = typename Types::string_type;
        using object = typename Types::template object_type<
            string, basic_json<Types, Alloc>, Alloc<std::pair<const string, basic_json<Types, Alloc>>>>;
        using array = typename Types::template array_type<basic_json<Types, Alloc>,
            Alloc<basic_json<Types, Alloc>>>;

        basic_json() noexcept
            : m_json_value{}, m_value_t{} {}
        basic_json(number n) noexcept
            : m_json_value{n}, m_value_t{value_t::_NUMBER} {}
        basic_json(boolean b) noexcept
            : m_json_value(b), m_value_t{value_t::_BOOLEAN} {}
        basic_json(const string& s) noexcept
            : m_json_value(s), m_value_t{value_t::_STRING} {}
        basic_json(const object& o) noexcept
            : m_json_value(o), m_value_t{value_t::_OBJECT} {}
        basic_json(const array& a) noexcept
            : m_json_value(a), m_value_t{value_t::_ARRAY} {}

        basic_json(const basic_json& other) noexcept
            : m_value_t{other.m_value_t} {
            switch (m_value_t) {
                case value_t::_NULL:
                    m_json_value = {};
                    break;
                case value_t::_NUMBER:
                    m_json_value = other.m_json_value.m_number;
                    break;
                case value_t::_BOOLEAN:
                    m_json_value = other.m_json_value.m_boolean;
                    break;
                case value_t::_STRING:
                    m_json_value = *other.m_json_value.m_string;
                    break;
                case value_t::_OBJECT:
                    m_json_value = *other.m_json_value.m_object;
                    break;
                case value_t::_ARRAY:
                    m_json_value = *other.m_json_value.m_array;
                    break;
            }
        }

        basic_json(basic_json&& other) noexcept
            : m_json_value{std::exchange(other.m_json_value, {})}, m_value_t{std::exchange(other.m_value_t, {})} {}

        ~basic_json() noexcept {
            switch (m_value_t) {
                case value_t::_NULL:
                case value_t::_NUMBER:
                case value_t::_BOOLEAN:
                    break;
                case value_t::_STRING:
                    destroy_heap_object<string>(m_json_value.m_string);
                    break;
                case value_t::_OBJECT:
                    destroy_heap_object<object>(m_json_value.m_object);
                    break;
                case value_t::_ARRAY:
                    destroy_heap_object<array>(m_json_value.m_array);
                    break;
            }
        }

        auto operator=(const basic_json& other) noexcept -> basic_json& {
            if (this != &other) {
                auto other_copy = other;
                std::swap(m_json_value, other_copy.m_json_value);
                std::swap(m_value_t, other_copy.m_value_t);
            }
            return *this;
        }

        auto operator=(basic_json&& other) noexcept -> basic_json& {
            if (this != &other) {
                m_json_value = std::exchange(other.m_json_value, {});
                m_value_t = std::exchange(other.m_value_t, {});
            }
            return *this;
        }

        auto operator[](typename array::size_type idx) -> basic_json& {
            return (*m_json_value.m_array)[idx];
        }

        auto operator[](typename array::size_type idx) const -> basic_json& {
            return (*m_json_value.m_array)[idx];
        }

        auto operator[](const typename object::key_type& key) -> basic_json& {
            return (*m_json_value.m_object)[key];
        }

        auto operator[](const typename object::key_type& key) const -> basic_json& {
            return (*m_json_value.m_object)[key];
        }

        auto swap(basic_json& other) noexcept -> void {
            std::swap(m_json_value, other.m_json_value);
            std::swap(m_value_t, other.m_value_t);
        }

        auto dump(const std::size_t& indent = 0) const noexcept -> std::string {
            return do_dump(indent, 1) + "\n";
        }

        auto dump(std::ostream& os, const std::size_t& indent = 0) const noexcept -> void {
            os << dump(indent);
        }

        friend auto operator==(const basic_json& js1, const basic_json& js2) noexcept -> bool {
            if (js1.m_value_t != js2.m_value_t) {
                return false;
            } else {
                auto is_equal = false;
                switch (js1.m_value_t) {
                    case value_t::_NULL:
                        is_equal = true;
                        break;
                    case value_t::_NUMBER:
                        is_equal = js1.m_json_value.m_number == js2.m_json_value.m_number;
                        break;
                    case value_t::_BOOLEAN:
                        is_equal = js1.m_json_value.m_boolean == js2.m_json_value.m_boolean;
                        break;
                    case value_t::_STRING:
                        is_equal = *js1.m_json_value.m_string == *js2.m_json_value.m_string;
                        break;
                    case value_t::_OBJECT:
                        is_equal = *js1.m_json_value.m_object == *js2.m_json_value.m_object;
                        break;
                    case value_t::_ARRAY:
                        is_equal = *js1.m_json_value.m_array == *js2.m_json_value.m_array;
                        break;
                }
                return is_equal;
            }
        }

        friend auto operator<<(std::ostream& os, const basic_json& js) noexcept -> std::ostream& {
            os << js.do_dump(0, 1);
            return os;
        }

        friend auto swap(basic_json& js1, basic_json& js2) noexcept -> void {
            js1.swap(js2);
        }

    private:
        template<typename T, typename... Args>
        static auto construct_heap_object(Args&& ...args) noexcept -> T* {
            using alloc_traits = std::allocator_traits<Alloc<T>>;
            auto alloc = Alloc<T>{};
            const auto ptr = alloc_traits::allocate(alloc, 1);
            alloc_traits::construct(alloc, ptr, std::forward<Args>(args)...);
            return ptr;
        }

        template<typename T>
        static auto destroy_heap_object(T* value) noexcept -> void {
            using alloc_traits = std::allocator_traits<Alloc<T>>;
            auto alloc = Alloc<T>{};
            alloc_traits::destroy(alloc, value);
            alloc_traits::deallocate(alloc, value, 1);
        }

        auto do_dump(const std::size_t& indent, const std::size_t& level) const noexcept -> std::string {
            const auto newline_str = std::string{(indent != 0) ? "\n" : ""};
            const auto indent_level = std::string(level * indent, ' ');
            auto str = std::string{};
            auto i = 0u;
            switch (m_value_t) {
                case value_t::_NULL:
                    str += "null";
                    break;
                case value_t::_NUMBER:
                    str += std::to_string(m_json_value.m_number);
                    break;
                case value_t::_BOOLEAN:
                    str += m_json_value.m_boolean ? "true" : "false";
                    break;
                case value_t::_STRING:
                    str += "\"" + *m_json_value.m_string + "\"";
                    break;
                case value_t::_OBJECT:
                    str += "{" + newline_str;
                    for (const auto& [key, val] : *m_json_value.m_object) {
                        str += indent_level + "\"" + key + "\": " + val.do_dump(indent, level + 1);
                        if (++i < m_json_value.m_object->size()) {
                            str += ", ";
                        }
                        str += newline_str;
                    }
                    str += std::string((level - 1) * indent, ' ') + "}";
                    break;
                case value_t::_ARRAY:
                    str += "[" + newline_str;
                    for (const auto& val : *m_json_value.m_array) {
                        str += indent_level + val.do_dump(indent, level + 1);
                        if (++i < m_json_value.m_array->size()) {
                            str += ", ";
                        }
                        str += newline_str;
                    }
                    str += std::string((level - 1) * indent, ' ') + "]";
                    break;
            }
            return str;
        }

        enum class value_t: std::uint8_t {
            _NULL,
            _NUMBER,
            _BOOLEAN,
            _STRING,
            _OBJECT,
            _ARRAY
        };

        union json_value {
            null m_null;
            number m_number;
            boolean m_boolean;
            string* m_string;
            object* m_object;
            array* m_array;

            json_value() noexcept
                : m_null{} {}
            json_value(number n) noexcept
                : m_number{n} {}
            json_value(boolean b) noexcept
                : m_boolean(b) {}
            json_value(const string& s) noexcept
                : m_string(construct_heap_object<string>(s)) {}
            json_value(const object& o) noexcept
                : m_object(construct_heap_object<object>(o)) {}
            json_value(const array& a) noexcept
                : m_array(construct_heap_object<array>(a)) {}
        };

        json_value m_json_value;
        value_t m_value_t;
    };

    template <typename Types = types, template<typename> class Alloc = std::allocator, typename ...Args>
    basic_json(Args&&...) -> basic_json<Types, Alloc>;

    // template <typename JsonType>
    // auto parse(std::string_view&& str) -> JsonType {
    //     return json_parser<string_reader, JsonType>{std::forward<std::string_view>(str)}.parse();
    // }
}

#endif
