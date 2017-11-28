#pragma once

#include <text_view_detail/ranges_concepts.hpp>

namespace ranges {

template<class T>
struct basic_mixin {
    typename T::mixin t_;
    auto& get() & { return t_; }
    auto&& get() && { return std::move(t_); }
    const auto& get() const & { return t_; }
    const auto&& get() const && { return std::move(t_); }
};

template<class Cursor>
class basic_iterator : public Cursor {
public:
    using value_type = std::decay_t<decltype(std::declval<Cursor&>().read())>;
    // typename Cursor::value_type;
    using difference_type = int; // typename Cursor::difference_type;
    using pointer = value_type*; // typename Cursor::pointer;
    using reference = value_type&; // typename Cursor::reference;
    using iterator_category = std::forward_iterator_tag;

    basic_iterator() = default;
    basic_iterator(basic_iterator&&) = default;
    basic_iterator(const basic_iterator&) = default;
    basic_iterator& operator=(basic_iterator&&) = default;
    basic_iterator& operator=(const basic_iterator&) = default;

    using Cursor::Cursor;
//    template<class... Ts, class = std::enable_if_t<stdx::is_constructible_v<Cursor, Ts&&...>>>
//    basic_iterator(Ts&&... ts) : Cursor(std::forward<Ts>(ts)...) {}

    decltype(auto) operator*() const { return Cursor::read(); }
    decltype(auto) operator->() const { return Cursor::arrow(); }
    basic_iterator& operator++() { Cursor::next(); return *this; }
    basic_iterator operator++(int) { auto r(*this); ++*this; return r; }
    bool operator==(const basic_iterator& rhs) const { return Cursor::equal(rhs); }
    bool operator!=(const basic_iterator& rhs) const { return !Cursor::equal(rhs); }
};

} // namespace ranges
