#ifndef JSON_HPP
#define JSON_HPP

#include <cstddef>
#include <initializer_list>
#include <map>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

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

        constexpr auto get_allocator() -> allocator_type {
            return allocator_type{};
        }

        constexpr basic_json() noexcept = default;

        template <typename T>
        requires (
            std::is_same_v<std::remove_cvref_t<T>, null> or std::is_same_v<std::remove_cvref_t<T>, number> or
            std::is_same_v<std::remove_cvref_t<T>, boolean> or std::is_same_v<std::remove_cvref_t<T>, string> or
            std::is_same_v<std::remove_cvref_t<T>, object> or std::is_same_v<std::remove_cvref_t<T>, array>
        )
        explicit basic_json(T&& t) noexcept
            : m_json_value{t} {
            if constexpr (std::is_same_v<std::remove_cvref_t<T>, null>) {
                m_value_t = value_t::_NULL;
            } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, number>) {
                m_value_t = value_t::_NUMBER;
            } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, boolean>) {
                m_value_t = value_t::_BOOLEAN;
            } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, string>) {
                m_value_t = value_t::_STRING;
            } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, object>) {
                m_value_t = value_t::_OBJECT;
            } else {
                m_value_t = value_t::_ARRAY;
            }
        }

        basic_json(size_type count, const basic_json& val)
            : m_value_t{value_t::_ARRAY} {
            m_json_value.m_array = construct_heap_object<Alloc<array>, array>(count, val);
        }

        basic_json(std::initializer_list<value_type> il)
            : m_value_t{value_t::_ARRAY} {
            m_json_value.m_array = construct_heap_object<Alloc<array>, array>(il.begin(), il.end());
        }

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
                case value_t::_NULL:
                case value_t::_NUMBER:
                case value_t::_BOOLEAN:
                    break;
                case value_t::_STRING:
                    destroy_heap_object<Alloc<string>, string>(m_json_value.m_string);
                    break;
                case value_t::_OBJECT:
                    destroy_heap_object<Alloc<object>, object>(m_json_value.m_object);
                    break;
                case value_t::_ARRAY:
                    destroy_heap_object<Alloc<array>, array>(m_json_value.m_array);
                    break;
            }
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
                case value_t::_NUMBER:
                case value_t::_BOOLEAN:
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
                case value_t::_NUMBER:
                case value_t::_BOOLEAN:
                default:
                    return 1;
            }
        }

        auto empty() const noexcept -> bool {
            return size() == 0;
        }

        auto clear() noexcept -> void {
            switch (m_value_t) {
                case value_t::_NULL:
                    break;
                case value_t::_STRING:
                    m_json_value.m_string->clear();
                case value_t::_OBJECT:
                    m_json_value.m_object->clear();
                case value_t::_ARRAY:
                    m_json_value.m_array->clear();
                    break;
                case value_t::_NUMBER:
                case value_t::_BOOLEAN:
                default:
                    m_json_value = {};
                    break;
            }
        }

        template <typename ...Args>
        auto emplace_back(Args&& ...args) -> reference {
            return m_json_value.m_array->emplace_back(std::forward<Args>(args)...);
        }

        auto push_back(const value_type& js) -> void {
            m_json_value.m_array->push_back(js);
        }

        auto push_back(value_type&& js) -> void {
            m_json_value.m_array->push_back(std::forward<value_type>(js));
        }

        auto pop_back() -> void {
            m_json_value.m_array->pop_back();
        }

        auto operator[](size_type n) -> reference {
            return (*m_json_value.m_array)[n];
        }

        auto operator[](size_type n) const -> const_reference {
            return (*m_json_value.m_array)[n];
        }

        auto at(size_type n) -> reference {
            return m_json_value.m_array->at(n);
        }

        auto at(size_type n) const -> const_reference {
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

        friend auto swap(basic_json& js1, basic_json& js2) noexcept -> void {
            js1.swap(js2);
        }

    private:
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
                : m_null{} {}
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