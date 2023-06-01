#ifndef CJSON_ITERATOR_HPP
#define CJSON_ITERATOR_HPP

#include <cstddef>
#include <iterator>

namespace cjson {
    enum class iter_value_t: std::uint8_t {
        _SCALAR,
        _OBJECT,
        _ARRAY
    };

    template <typename Basic_Json>
    class iter {
        friend Basic_Json;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Basic_Json;
        using reference = value_type&;
        using pointer = value_type*;
        using difference_type = std::ptrdiff_t;

        iter() noexcept = default;
        iter(const iter&) noexcept = default;

        auto operator=(const iter&) noexcept -> iter& = default;

        auto operator*() const -> reference {
            switch (m_iter_value_t) {
                case iter_value_t::_OBJECT:
                    return m_iter_value.m_object_iter->second;
                case iter_value_t::_ARRAY:
                    return *m_iter_value.m_array_iter;
                case iter_value_t::_SCALAR:
                default:
                    return *m_iter_value.m_scalar_value;
            }
        }

        auto operator->() const -> pointer {
            switch (m_iter_value_t) {
                case iter_value_t::_OBJECT:
                    return &(*m_iter_value.m_object_iter);
                case iter_value_t::_ARRAY:
                    return &(*m_iter_value.m_array_iter);
                case iter_value_t::_SCALAR:
                default:
                    return m_iter_value.m_scalar_value;
            }
        }

        auto operator++() -> iter& {
            switch (m_iter_value_t) {
                case iter_value_t::_OBJECT:
                    ++m_iter_value.m_object_iter;
                    break;
                case iter_value_t::_ARRAY:
                    ++m_iter_value.m_array_iter;
                    break;
                case iter_value_t::_SCALAR:
                    ++m_iter_value.m_scalar_value;
                    break;
            }
            return *this;
        }

        auto operator++(int) -> iter {
           auto self = *this;
            ++(*this);
            return self;
        }

        auto operator--() -> iter& {
            switch (m_iter_value_t) {
                case iter_value_t::_OBJECT:
                    --m_iter_value.m_object_iter;
                    break;
                case iter_value_t::_ARRAY:
                    --m_iter_value.m_array_iter;
                    break;
                case iter_value_t::_SCALAR:
                    --m_iter_value.m_scalar_value;
                    break;
            }
            return *this;
        }

        auto operator--(int) -> iter {
           auto self = *this;
            --(*this);
            return self;
        }

        auto operator==(const iter& other) const -> bool {
            if (m_iter_value_t != other.m_iter_value_t) {
                return false;
            }

            switch (m_iter_value_t) {
                case iter_value_t::_SCALAR:
                    return m_iter_value.m_scalar_value == other.m_iter_value.m_scalar_value;
                case iter_value_t::_OBJECT:
                    return m_iter_value.m_object_iter == other.m_iter_value.m_object_iter;
                case iter_value_t::_ARRAY:
                    return m_iter_value.m_array_iter == other.m_iter_value.m_array_iter;
                default:
                    return false;
            }
        }

    private:
        iter(const pointer json_ptr) noexcept
            : m_iter_value{json_ptr}, m_iter_value_t{iter_value_t::_SCALAR} {}
        iter(const typename value_type::object::iterator& obj_iter) noexcept
            : m_iter_value{obj_iter}, m_iter_value_t{iter_value_t::_OBJECT} {}
        iter(const typename value_type::array::iterator& array_iter) noexcept
                : m_iter_value{array_iter}, m_iter_value_t{iter_value_t::_ARRAY} {}

        union iter_value {
            pointer m_scalar_value;
            typename value_type::object::iterator m_object_iter;
            typename value_type::array::iterator m_array_iter;

            iter_value() noexcept = default;
            iter_value(const pointer json_ptr) noexcept
                : m_scalar_value{json_ptr} {}
            iter_value(const typename value_type::object::iterator& obj_iter) noexcept
                : m_object_iter{obj_iter} {}
            iter_value(const typename value_type::array::iterator& array_iter) noexcept
                : m_array_iter{array_iter} {}
        };

        iter_value m_iter_value;
        iter_value_t m_iter_value_t;
    };
}


#endif
