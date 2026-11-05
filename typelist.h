#pragma once

#include <cstddef>
#include <utility>

// As I understand the task, we should not use standard type_traits at all

namespace ct::tl {

template <typename... Types>
struct TypeList {};

template <typename Type, typename List>
struct contains_impl;

template <typename Type, template <typename...> typename List, typename... Types>
struct contains_impl<Type, List<Types...>> {
  static constexpr bool value = (std::is_same_v<Type, Types> || ...);
};

template <typename Type, typename List>
inline constexpr bool contains = contains_impl<Type, List>::value;

template <typename List>
struct flip_impl;

template <template <typename...> typename List, typename T1, typename T2>
struct flip_impl<List<T1, T2>> {
  using type = List<T2, T1>;
};

template <typename List>
using flip = flip_impl<List>::type;

template <typename List>
struct flip_all_impl;

template <template <typename...> typename List, typename... Types>
struct flip_all_impl<List<Types...>> {
  using type = List<flip<Types>...>;
};

template <typename List>
using flip_all = flip_all_impl<List>::type;

template <std::size_t VALUE>
struct index {
  static constexpr std::size_t value = VALUE;
};

template <typename List>
struct count_impl;

template <template <typename...> typename List, typename... Types>
struct count_impl<List<Types...>> {
  static constexpr std::size_t value = sizeof...(Types);
};

template <typename List>
inline constexpr std::size_t count = count_impl<List>::value;

template <typename List, typename Seq>
struct enumerate_impl;

template <template <typename...> typename List, typename... Types, std::size_t... INDICES>
struct enumerate_impl<List<Types...>, std::index_sequence<INDICES...>> {
  using type = List<List<Types, index<INDICES>>...>;
};

template <typename List>
using enumerate = enumerate_impl<List, std::make_index_sequence<count<List>>>::type;

template <typename... Types>
struct inherit : Types... {};

template <typename Map, typename Key>
struct map_find_index_impl;

template <template <typename...> typename List, typename... Lists, typename Key>
struct map_find_index_impl<List<Lists...>, Key> {
  template <template <typename...> typename InnerList, typename Index>
  static Index f(InnerList<Key, Index>*);

  using derived = inherit<Lists...>;

  // derived* is implicitly convertible to List* for each List in Lists
  using type = decltype(f(static_cast<derived*>(nullptr)));
};

template <typename Map, typename Key>
using map_find_index = map_find_index_impl<Map, Key>::type;

template <typename Type, typename List>
inline constexpr std::size_t index_of_unique = map_find_index<enumerate<List>, Type>::value;

template <template <typename...> typename List, typename... Types1, typename... Types2>
List<Types1..., Types2...> operator+(List<Types1...>, List<Types2...>);

template <typename List1, typename List2>
struct concat2_impl;

template <template <typename...> typename List, typename... Types1, typename... Types2>
struct concat2_impl<List<Types1...>, List<Types2...>> {
  using type = List<Types1..., Types2...>;
};

template <typename List1, typename List2>
using concat2 = concat2_impl<List1, List2>::type;

template <typename... Lists>
using concat = decltype((Lists{} + ...));

template <typename List>
struct flatten_impl;

template <template <typename...> typename List, typename T>
struct flatten_one_impl {
  using type = List<T>;
};

template <template <typename...> typename List, typename Type>
using flatten_one = flatten_one_impl<List, Type>::type;

template <template <typename...> typename List, template <typename...> typename InternalList, typename... Types>
struct flatten_one_impl<List, InternalList<Types...>> {
  using type = concat<flatten_one<List, Types>...>;
};

template <template <typename...> typename List, template <typename...> typename InternalList>
struct flatten_one_impl<List, InternalList<>> {
  using type = List<>;
};

template <template <typename...> typename List, typename... Types>
struct flatten_impl<List<Types...>> {
  using type = concat<List<>, flatten_one<List, Types>...>;
};

template <typename List>
using flatten = flatten_impl<List>::type;

template <bool B>
struct conditional_impl;

template <>
struct conditional_impl<true> {
  template <typename T, typename F>
  using type = T;
};

template <>
struct conditional_impl<false> {
  template <typename T, typename F>
  using type = F;
};

template <bool B, typename T, typename F>
using conditional = conditional_impl<B>::template type<T, F>;

template <typename List, typename Seq>
struct divide_impl;

template <template <typename...> typename List, typename... Types, std::size_t... INDICES> // check
struct divide_impl<List<Types...>, std::index_sequence<INDICES...>> {
  static constexpr std::size_t mid = sizeof...(Types) / 2;
  using first = concat<conditional<(INDICES < mid), List<Types>, List<>>...>;
  using second = concat<conditional<(INDICES >= mid), List<Types>, List<>>...>;
};

template <typename List>
using divide = divide_impl<List, std::make_index_sequence<count<List>>>;

template <template <typename, typename> typename Compare, typename First, typename Second>
struct merge_impl;

template <template <typename, typename> typename Compare, template <typename...> typename List, typename... Types>
struct merge_impl<Compare, List<>, List<Types...>> {
  using type = List<Types...>;
};

template <template <typename, typename> typename Compare, template <typename...> typename List, typename... Types>
struct merge_impl<Compare, List<Types...>, List<>> {
  using type = List<Types...>;
};

template <template <typename, typename> typename Compare, template <typename...> typename List>
struct merge_impl<Compare, List<>, List<>> {
  using type = List<>;
};

template <bool B, template <typename, typename> typename Compare, typename First, typename Second>
struct merge_conditional_impl;

template <
    template <typename, typename> typename Compare,
    template <typename...> typename List,
    typename Head1,
    typename... Types1,
    typename Head2,
    typename... Types2>
struct merge_conditional_impl<true, Compare, List<Head1, Types1...>, List<Head2, Types2...>> {
  using type = concat2<List<Head1>, typename merge_impl<Compare, List<Types1...>, List<Head2, Types2...>>::type>;
};

template <
    template <typename, typename> typename Compare,
    template <typename...> typename List,
    typename Head1,
    typename... Types1,
    typename Head2,
    typename... Types2>
struct merge_conditional_impl<false, Compare, List<Head1, Types1...>, List<Head2, Types2...>> {
  using type = concat2<List<Head2>, typename merge_impl<Compare, List<Head1, Types1...>, List<Types2...>>::type>;
};

template <bool B, template <typename, typename> typename Compare, typename First, typename Second>
using merge_conditional = merge_conditional_impl<B, Compare, First, Second>::type;

template <
    template <typename, typename> typename Compare,
    template <typename...> typename List,
    typename Head1,
    typename... Types1,
    typename Head2,
    typename... Types2>
struct merge_impl<Compare, List<Head1, Types1...>, List<Head2, Types2...>> {
  using type = merge_conditional<Compare<Head1, Head2>::value, Compare, List<Head1, Types1...>, List<Head2, Types2...>>;
};

template <template <typename, typename> typename Compare, typename First, typename Second>
using merge = merge_impl<Compare, First, Second>::type;

template <template <typename, typename> typename Compare, typename List>
struct merge_sort_impl;

template <template <typename, typename> typename Compare, template <typename...> typename List>
struct merge_sort_impl<Compare, List<>> {
  using type = List<>;
};

template <template <typename, typename> typename Compare, template <typename...> typename List, typename T>
struct merge_sort_impl<Compare, List<T>> {
  using type = List<T>;
};

template <template <typename, typename> typename Compare, typename List>
using merge_sort = merge_sort_impl<Compare, List>::type;

template <template <typename, typename> typename Compare, template <typename...> typename List, typename... T>
struct merge_sort_impl<Compare, List<T...>> {
  using divided = divide<List<T...>>;
  using first_sorted = merge_sort<Compare, typename divided::first>;
  using second_sorted = merge_sort<Compare, typename divided::second>;
  using type = merge<Compare, first_sorted, second_sorted>;
};

} // namespace ct::tl
