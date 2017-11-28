#pragma once

#include <iterator>
#include <type_traits>
#include <text_view_detail/adl_customization.hpp>

namespace stdx {
    template<class...> using void_t = void;

    template<class...> using true_t = std::true_type;
    template<class...> constexpr bool true_v = true;

    template<class...> using false_t = std::false_type;
    template<class...> constexpr bool false_v = false;

    template<class... Ts> constexpr bool is_base_of_v = std::is_base_of<Ts...>::value;
    template<class... Ts> constexpr bool is_constructible_v = std::is_constructible<Ts...>::value;
    template<class... Ts> constexpr bool is_convertible_v = std::is_convertible<Ts...>::value;

    template<class R> using iterator_t = std::decay_t<decltype(std::declval<R>().begin())>;
    template<class I> using iterator_value_type_t = typename std::iterator_traits<I>::value_type;
    template<class I> using iterator_difference_type_t = typename std::iterator_traits<I>::difference_type;

    template<class R> auto size_impl(const R& r, int) -> decltype(r.begin(), void(), size_t()) {
        return std::distance(r.begin(), r.end());
    }
    template<class R> auto size_impl(const R& r, long) {
        return std::size(r);
    }
    template<class R> auto size(const R& r) { return stdx::size_impl(r, 0); }

} // namespace stdx


template<class I, class = void> struct is_Iterator : std::false_type {};
template<class I> struct is_Iterator<I, std::enable_if_t<
    stdx::true_v<typename std::iterator_traits<I>::iterator_category>
>> : std::true_type {};
template<class I> constexpr bool is_Iterator_v = is_Iterator<I>::value;

template<class I, class = void> struct is_InputIterator : std::false_type {};
template<class I> struct is_InputIterator<I, std::enable_if_t<
    stdx::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<I>::iterator_category>
>> : std::true_type {};
template<class I> constexpr bool is_InputIterator_v = is_InputIterator<I>::value;

template<class I, class = void> struct is_ForwardIterator : std::false_type {};
template<class I> struct is_ForwardIterator<I, std::enable_if_t<
    stdx::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<I>::iterator_category>
>> : std::true_type {};
template<class I> constexpr bool is_ForwardIterator_v = is_ForwardIterator<I>::value;

template<class I, class = void> struct is_BidirectionalIterator : std::false_type {};
template<class I> struct is_BidirectionalIterator<I, std::enable_if_t<
    stdx::is_base_of_v<std::bidirectional_iterator_tag, typename std::iterator_traits<I>::iterator_category>
>> : std::true_type {};
template<class I> constexpr bool is_BidirectionalIterator_v = is_BidirectionalIterator<I>::value;


template<class I, class T, class = void> struct is_OutputIterator : std::false_type {};
template<class I, class T> struct is_OutputIterator<I, T, std::enable_if_t<
    stdx::is_base_of_v<std::output_iterator_tag, typename std::iterator_traits<I>::iterator_category> &&
    stdx::is_convertible_v<decltype(*std::declval<const I&>()), T>
>> : std::true_type {};
template<class I, class T> constexpr bool is_OutputIterator_v = is_OutputIterator<I, T>::value;


template<class T, class U, class = void> struct is_WeaklyEqualityComparableWith : std::false_type {};
template<class T, class U> struct is_WeaklyEqualityComparableWith<T, U, std::enable_if_t<
    stdx::is_convertible_v<decltype(std::declval<const T&>() == std::declval<const U&>()), bool> &&
    stdx::is_convertible_v<decltype(std::declval<const T&>() != std::declval<const U&>()), bool> &&
    stdx::is_convertible_v<decltype(std::declval<const U&>() == std::declval<const T&>()), bool> &&
    stdx::is_convertible_v<decltype(std::declval<const U&>() != std::declval<const T&>()), bool>
>> : std::true_type {};
template<class T, class U> constexpr bool is_WeaklyEqualityComparableWith_v = is_WeaklyEqualityComparableWith<T, U>::value;


template<class S, class I> struct is_Sentinel : std::bool_constant<
    is_Iterator_v<I> &&
    is_WeaklyEqualityComparableWith_v<S, I>
> {};
template<class S, class I> constexpr bool is_Sentinel_v = is_Sentinel<S, I>::value;


template<class R, class = void> struct is_Range : std::false_type {};
template<class R> struct is_Range<R, std::enable_if_t<
    is_WeaklyEqualityComparableWith_v<
        decltype(std::experimental::text_detail::adl_begin(std::declval<R&>())),
        decltype(std::experimental::text_detail::adl_end(std::declval<R&>()))
    >
>> : std::true_type {};
template<class R> constexpr bool is_Range_v = is_Range<R>::value;

template<class R, class = void> struct is_InputRange : std::false_type {};
template<class R> struct is_InputRange<R, std::enable_if_t<
    is_Range_v<R> &&
    is_InputIterator_v<decltype(std::experimental::text_detail::adl_begin(std::declval<R&>()))>
>> : std::true_type {};
template<class R> constexpr bool is_InputRange_v = is_InputRange<R>::value;
