#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike = requires(T t){
  //base
  { t.size() } -> std::convertible_to<std::size_t>;
  { t.reserve(std::size_t{}) };
  { t.clear() };

  // Итераторы
  { t.begin() } -> std::input_or_output_iterator;
  { t.end() } -> std::sentinel_for<typename T::iterator>;
  
  // Константные версии
  { t.cbegin() } -> std::input_iterator;
  { t.cend() } -> std::sentinel_for<typename T::const_iterator>;

};

template <typename Iter>
concept BookIterator = std::random_access_iterator<Iter>;

template <typename S, typename I>
concept BookSentinel = std::sentinel_for<S, I>;

template <typename P, typename Arg>
concept BookPredicate = std::predicate<P, Arg>;

template <typename F, typename T, typename U>
concept BookComparator = std::strict_weak_order<F, T, U>;

}  // namespace bookdb
