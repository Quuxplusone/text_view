// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_BASIC_VIEW_HPP // {
#define TEXT_VIEW_BASIC_VIEW_HPP


#include <utility>
#include <text_view_detail/ranges_concepts.hpp>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {

template<class IT, class ST = IT, class = std::enable_if_t<is_Iterator_v<IT> && is_Sentinel_v<ST, IT>>>
class basic_view
{
public:
    using iterator = IT;
    using sentinel = ST;

    basic_view() = default;
    basic_view(IT first, ST last)
        : first{first}, last{last} {}

    template<class IT2, class ST2, class = std::enable_if_t<
        stdx::is_constructible_v<IT, IT2&&> &&
        stdx::is_constructible_v<ST, ST2&&>
    >>
    basic_view(IT2 first, ST2 last)
        : first(std::move(first)), last(std::move(last)) {}

    template<class IT2, class ST2, class = std::enable_if_t<
        stdx::is_constructible_v<IT, IT2&&> &&
        stdx::is_constructible_v<ST, ST2&&>
    >>
    basic_view(const basic_view<IT2, ST2> &o)
        : first(o.begin()), last(o.end()) {}

    IT begin() const { return first; }
    ST end() const { return last; }

private:
    IT first = {};
    ST last = {};
};


template<class IT, class ST, class = std::enable_if_t<is_Iterator_v<IT> && is_Sentinel_v<ST, IT>>>
auto make_basic_view(IT first, ST last) {
    return basic_view<IT, ST>{std::move(first), std::move(last)};
}


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_BASIC_VIEW_HPP
