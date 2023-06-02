#ifndef CJSON_HPP
#define CJSON_HPP

#include <bits/iterator_concepts.h>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "cjson_iterator.hpp"

namespace cjson {
    struct json_types {
        using number_type = double;
        using boolean_type = bool;
        using string_type = std::string;

        template <typename Key, typename Val, typename Alloc>
        using object_type = std::map<Key, Val, std::less<string_type>, Alloc>;

        template <typename Val, typename Alloc>
        using array_type = std::vector<Val, Alloc>;
    };

    enum class value_t: std::uint8_t {
        _NULL,
        _NUMBER,
        _BOOLEAN,
        _STRING,
        _OBJECT,
        _ARRAY
    };

    template <typename Types, template<typename> class Alloc = std::allocator>
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

        using value_type = basic_json;
        using reference = value_type&;
        using const_reference = const value_type&;
        using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;
        using allocator_type = Alloc<basic_json>;
        using iterator = iter<basic_json>;
        using const_iterator = iter<const basic_json>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr auto get_allocator() -> allocator_type {
            return allocator_type{};
        }

        constexpr basic_json() noexcept = default;

        explicit basic_json(std::nullptr_t n)
            : m_json_value{n}, m_value_t{value_t::_NULL} {}

        template <typename NUMBER>
        requires std::convertible_to<std::remove_cvref_t<NUMBER>, number>
        explicit basic_json(NUMBER n)
            : m_json_value{number(n)}, m_value_t{value_t::_NUMBER} {}

        explicit basic_json(boolean&& b)
            : m_json_value{b}, m_value_t{value_t::_BOOLEAN} {}

        template <typename STRING>
        requires std::convertible_to<std::remove_cvref_t<STRING>, string>
        explicit basic_json(const STRING& s)
            : m_json_value{string{s}}, m_value_t{value_t::_STRING} {}

        template <typename STRING>
        requires std::convertible_to<std::remove_cvref_t<STRING>, string>
        explicit basic_json(STRING&& s)
            : m_json_value{string{s}}, m_value_t{value_t::_STRING} {}

        explicit basic_json(const object& o)
            : m_json_value{o}, m_value_t{value_t::_OBJECT} {}

        explicit basic_json(object&& o)
            : m_json_value{o}, m_value_t{value_t::_OBJECT} {}

        explicit basic_json(const array& a)
            : m_json_value{a}, m_value_t{value_t::_ARRAY} {}

        explicit basic_json(array&& a)
            : m_json_value{a}, m_value_t{value_t::_ARRAY} {}

        basic_json(size_type count, const value_type& val)
            : m_value_t{value_t::_ARRAY} {
            m_json_value.m_array = construct_heap_object<Alloc<array>, array>(count, val);
        }

        template <std::input_iterator InputIterator>
        requires std::is_same_v<std::iter_value_t<InputIterator>, typename array::value_type>
        basic_json(InputIterator first, InputIterator last)
            : m_value_t{value_t::_ARRAY} {
            m_json_value.m_array = construct_heap_object<Alloc<array>, array>(first, last);
        }

        template <std::input_iterator InputIterator>
        requires std::is_same_v<std::iter_value_t<InputIterator>, typename object::value_type>
        basic_json(InputIterator first, InputIterator last)
            : m_value_t{value_t::_OBJECT} {
            m_json_value.m_object = construct_heap_object<Alloc<object>, object>(first, last);
        }

        basic_json(std::initializer_list<typename array::value_type> il)
            : basic_json(il.begin(), il.end()) {}

        basic_json(std::initializer_list<typename object::value_type> il)
            : basic_json(il.begin(), il.end()) {}

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
            : m_json_value{std::exchange(other.m_json_value, {})}
            , m_value_t{std::exchange(other.m_value_t, {})} {}

        auto operator=(const basic_json& other) noexcept -> basic_json& {
            if (this != &other) {
                auto other_copy = other;
                std::swap(m_json_value, other_copy.m_json_value);
                std::swap(m_value_t, other_copy.m_value_t);
            }
            return *this;
        }

        auto operator=(std::initializer_list<value_type> il) -> basic_json& {
            auto other = basic_json(il);
            std::swap(m_json_value, other.m_json_value);
            std::swap(m_value_t, other.m_value_t);
        }

        auto operator=(basic_json&& other) noexcept -> basic_json& {
            if (this != &other) {
                m_json_value = std::exchange(other.m_json_value, {});
                m_value_t = std::exchange(other.m_value_t, {});
            }
            return *this;
        }

        ~basic_json() noexcept {
            switch (m_value_t) {
                case value_t::_STRING:
                    destroy_heap_object<Alloc<string>, string>(m_json_value.m_string);
                    break;
                case value_t::_OBJECT:
                    destroy_heap_object<Alloc<object>, object>(m_json_value.m_object);
                    break;
                case value_t::_ARRAY:
                    destroy_heap_object<Alloc<array>, array>(m_json_value.m_array);
                    break;
                default:
                    break;
            }
            m_json_value = {};
            m_value_t = {};
        }

        auto begin() -> iterator {
            switch (m_value_t) {
                case value_t::_OBJECT:
                    return iterator{m_json_value.m_object->begin()};
                case value_t::_ARRAY:
                    return iterator{m_json_value.m_array->begin()};
                default:
                    return iterator{this};
            }
        }

        auto end() -> iterator {
            switch (m_value_t) {
                case value_t::_OBJECT:
                    return iterator{m_json_value.m_object->end()};
                case value_t::_ARRAY:
                    return iterator{m_json_value.m_array->end()};
                default:
                    return iterator{this + 1};
            }
        }

        auto begin() const -> const_iterator {
            switch (m_value_t) {
                case value_t::_OBJECT:
                    return const_iterator{m_json_value.m_object->begin()};
                case value_t::_ARRAY:
                    return const_iterator{m_json_value.m_array->begin()};
                default:
                    return const_iterator{this};
            }
        }

        auto end() const -> const_iterator {
            switch (m_value_t) {
                case value_t::_OBJECT:
                    return const_iterator{m_json_value.m_object->end()};
                case value_t::_ARRAY:
                    return const_iterator{m_json_value.m_array->end()};
                default:
                    return const_iterator{this + 1};
            }
        }

        auto cbegin() const -> const_iterator {
            switch (m_value_t) {
                case value_t::_OBJECT:
                    return const_iterator{m_json_value.m_object->begin()};
                case value_t::_ARRAY:
                    return const_iterator{m_json_value.m_array->begin()};
                default:
                    return const_iterator{this};
            }
        }

        auto cend() const -> const_iterator {
            switch (m_value_t) {
                case value_t::_OBJECT:
                    return const_iterator{m_json_value.m_object->end()};
                case value_t::_ARRAY:
                    return const_iterator{m_json_value.m_array->end()};
                default:
                    return const_iterator{this + 1};
            }
        }

        auto rbegin() -> reverse_iterator {
            return reverse_iterator{end()};
        }

        auto rend() -> reverse_iterator {
            return reverse_iterator{begin()};
        }

        auto rbegin() const -> const_reverse_iterator {
            return const_reverse_iterator{end()};
        }

        auto rend() const -> const_reverse_iterator {
            return const_reverse_iterator{begin()};
        }

        auto crbegin() const -> const_reverse_iterator {
            return const_reverse_iterator{end()};
        }

        auto crend() const -> const_reverse_iterator {
            return const_reverse_iterator{begin()};
        }

        auto swap(basic_json& other) noexcept -> void {
            std::swap(m_json_value, other.m_json_value);
            std::swap(m_value_t, other.m_value_t);
        }

        auto size() const noexcept -> size_type {
            switch (m_value_t) {
                case value_t::_NULL:
                    return 0;
                case value_t::_STRING:
                    return m_json_value.m_string->size();
                case value_t::_OBJECT:
                    return m_json_value.m_object->size();
                case value_t::_ARRAY:
                    return m_json_value.m_array->size();
                default:
                    return 1;
            }
        }

        auto max_size() const noexcept -> size_type {
            switch (m_value_t) {
                case value_t::_NULL:
                    return 0;
                case value_t::_STRING:
                    return m_json_value.m_string->max_size();
                case value_t::_OBJECT:
                    return m_json_value.m_object->max_size();
                case value_t::_ARRAY:
                    return m_json_value.m_array->max_size();
                default:
                    return 1;
            }
        }

        auto empty() const noexcept -> bool {
            return size() == 0;
        }

        template <typename... Args>
        auto emplace(const_iterator position, Args&& ...args) -> iterator {
            return iterator{m_json_value.m_array->emplace(position.m_iter_value.m_array_iter, std::forward<Args>(args)...)};
        }

        auto insert(const_iterator position, const typename array::value_type& value) -> iterator {
            return iterator{m_json_value.m_array->insert(position.m_iter_value.m_array_iter, value)};
        }

        auto insert(const_iterator position, typename array::value_type&& value) -> iterator {
            return iterator{m_json_value.m_array->insert(position.m_iter_value.m_array_iter, value)};
        }

        auto insert(const_iterator position, size_type n, const typename array::value_type& value) -> iterator {
            return iterator{m_json_value.m_array->insert(position.m_iter_value.m_array_iter, n, value)};
        }

        template <std::input_iterator InputIterator>
        requires std::is_same_v<std::iter_value_t<InputIterator>, typename array::value_type>
        auto insert(const_iterator position, InputIterator first, InputIterator last) -> iterator {
            return iterator{m_json_value.m_array->insert(position.m_iter_value.m_array_iter, first, last)};
        }

        auto insert(const_iterator position, std::initializer_list<typename array::value_type> il) -> iterator {
            return iterator{m_json_value.m_array->insert(position.m_iter_value.m_array_iter, il)};
        }

        auto erase(const_iterator position) -> iterator {
            switch (position.m_iter_value_t) {
                case iter_value_t::_OBJECT:
                    return iterator{m_json_value.m_object->erase(position.m_iter_value.m_object_iter)};
                case iter_value_t::_ARRAY:
                    return iterator{m_json_value.m_array->erase(position.m_iter_value.m_array_iter)};
                default:
                    clear();
                    return begin();
            }
        }

        auto erase(const_iterator first, const_iterator last) -> iterator {
            switch (first.m_iter_value_t) {
                case iter_value_t::_OBJECT:
                    return iterator{m_json_value.m_object->erase(first.m_iter_value.m_object_iter, last.m_iter_value.m_object_iter)};
                case iter_value_t::_ARRAY:
                    return iterator{m_json_value.m_array->erase(first.m_iter_value.m_array_iter, last.m_iter_value.m_array_iter)};
                default:
                    clear();
                    return begin();
            }
        }

        auto clear() noexcept -> void {
            switch (m_value_t) {
                case value_t::_STRING:
                    m_json_value.m_string->clear();
                    break;
                case value_t::_OBJECT:
                    m_json_value.m_object->clear();
                    break;
                case value_t::_ARRAY:
                    m_json_value.m_array->clear();
                    break;
                default:
                    m_json_value = {};
                    m_value_t = {};
                    break;
            }
        }

        template <std::input_iterator InputIterator>
        requires std::is_same_v<std::iter_value_t<InputIterator>, typename array::value_type>
        auto assign(InputIterator first, InputIterator last) -> void {
            m_json_value.m_array->assign(first, last);
        }

        auto assign(std::initializer_list<typename array::value_type> il) -> void {
            m_json_value.m_array->assign(il);
        }

        auto assign(size_type n, const typename array::value_type& value) -> void {
            m_json_value.m_array->assign(n, value);
        }

        auto front() -> reference {
            return *begin();
        }

        auto front() const -> const_reference {
            return *cbegin();
        }

        auto back() -> reference
            requires (requires (array a) { {a.back()} -> std::same_as<typename array::reference>; }) {
            return *(--end());
        }

        auto back() const -> const_reference
            requires (requires (array a) { {a.back()} -> std::same_as<typename array::const_reference>; }) {
            return *(--cend());
        }

        template <typename ...Args>
        requires (requires (array a) { {a.emplace_front()} -> std::same_as<typename array::reference>; })
        auto emplace_front(Args&& ...args) -> reference {
            return m_json_value.m_array->emplace_front(std::forward<Args>(args)...);
        }

        template <typename ...Args>
        requires (requires (array a) { {a.emplace_back()} -> std::same_as<typename array::reference>; })
        auto emplace_back(Args&& ...args) -> reference {
            return m_json_value.m_array->emplace_back(std::forward<Args>(args)...);
        }

        auto push_front(const value_type& js) -> void
            requires (requires (array a) { {a.push_front(js)} -> std::same_as<void>; }) {
            m_json_value.m_array->push_front(js);
        }

        auto push_front(value_type&& js) -> void
            requires (requires (array a) { {a.push_front(js)} -> std::same_as<void>; }) {
            m_json_value.m_array->push_front(std::forward<value_type>(js));
        }

        auto push_back(const value_type& js) -> void
            requires (requires (array a) { {a.push_back(js)} -> std::same_as<void>; }) {
            m_json_value.m_array->push_back(js);
        }

        auto push_back(value_type&& js) -> void
            requires (requires (array a) { {a.push_back(js)} -> std::same_as<void>; }) {
            m_json_value.m_array->push_back(std::forward<value_type>(js));
        }

        auto pop_front() -> void
            requires (requires (array a) { {a.pop_front()} -> std::same_as<void>; }) {
            m_json_value.m_array->pop_front();
        }

        auto pop_back() -> void
            requires (requires (array a) { {a.pop_back()} -> std::same_as<void>; }) {
            m_json_value.m_array->pop_back();
        }

        auto operator[](size_type n) -> reference
            requires (requires (array a) { {a.operator[](n)} -> std::same_as<typename array::reference>; }) {
            return (*m_json_value.m_array)[n];
        }

        auto operator[](size_type n) const -> const_reference
            requires (requires (const array a) { {a.operator[](n)} -> std::same_as<typename array::const_reference>; }) {
            return (*m_json_value.m_array)[n];
        }

        auto at(size_type n) -> reference
            requires (requires (array a) { {a.at(n)} -> std::same_as<typename array::reference>; }) {
            return m_json_value.m_array->at(n);
        }

        auto at(size_type n) const -> const_reference
            requires (requires (const array a) { {a.at(n)} -> std::same_as<typename array::const_reference>; }) {
            return m_json_value.m_array->at(n);
        }

        friend auto operator==(const basic_json& js1, const basic_json& js2) noexcept -> bool {
            if (js1.m_value_t != js2.m_value_t) {
                return false;
            } else {
                auto equal = false;
                switch (js1.m_value_t) {
                    case value_t::_NULL:
                        equal = true;
                        break;
                    case value_t::_NUMBER:
                        equal = js1.m_json_value.m_number == js2.m_json_value.m_number;
                        break;
                    case value_t::_BOOLEAN:
                        equal = js1.m_json_value.m_boolean == js2.m_json_value.m_boolean;
                        break;
                    case value_t::_STRING:
                        equal = *js1.m_json_value.m_string == *js2.m_json_value.m_string;
                        break;
                    case value_t::_OBJECT:
                        equal = *js1.m_json_value.m_object == *js2.m_json_value.m_object;
                        break;
                    case value_t::_ARRAY:
                        equal = *js1.m_json_value.m_array == *js2.m_json_value.m_array;
                        break;
                }
                return equal;
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

        template<typename A, typename T, typename... Args>
        static auto construct_heap_object(Args&& ...args) noexcept -> T* {
            using alloc_traits = std::allocator_traits<A>;
            auto alloc = A{};
            const auto ptr = alloc_traits::allocate(alloc, 1);
            alloc_traits::construct(alloc, ptr, std::forward<Args>(args)...);
            return ptr;
        }

        template<typename A, typename T>
        static auto destroy_heap_object(T* value) noexcept -> void {
            using alloc_traits = std::allocator_traits<A>;
            auto alloc = A{};
            alloc_traits::destroy(alloc, value);
            alloc_traits::deallocate(alloc, value, 1);
        }

        union json_value {
            null m_null;
            number m_number;
            boolean m_boolean;
            string* m_string;
            object* m_object;
            array* m_array;

            json_value() noexcept
                : json_value{null{}} {}
            json_value(null n) noexcept
                : m_null{n} {}
            json_value(number n) noexcept
                : m_number{n} {}
            json_value(boolean b) noexcept
                : m_boolean(b) {}
            json_value(const string& s) noexcept
                : m_string(construct_heap_object<Alloc<string>, string>(s)) {}
            json_value(const object& o) noexcept
                : m_object(construct_heap_object<Alloc<object>, object>(o)) {}
            json_value(const array& a) noexcept
                : m_array(construct_heap_object<Alloc<array>, array>(a)) {}
            json_value(string&& s) noexcept
                : m_string(construct_heap_object<Alloc<string>, string>(std::move(s))) {}
            json_value(object&& o) noexcept
                : m_object(construct_heap_object<Alloc<object>, object>(std::move(o))) {}
            json_value(array&& a) noexcept
                : m_array(construct_heap_object<Alloc<array>, array>(std::move(a))) {}
        };

        json_value m_json_value;
        value_t m_value_t;
    };
}


#endif
