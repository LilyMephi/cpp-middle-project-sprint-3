#pragma once

#include <algorithm>
#include <functional>

#include "book.hpp"
#include "concepts.hpp"

namespace bookdb {
auto YearBetween(int start_year, int end_year) {
    return [=](const Book &book) -> bool {
        int book_year = book.year;
        return book_year > start_year && book_year < end_year;
    };
}

auto RatingAbove(double rating_above) {
    return [=](const Book &book) -> bool { return book.rating > rating_above; };
}

auto LessAbove(double rating_above) {
    return [=](const Book &book) -> bool { return book.rating < rating_above; };
}

auto GenreIs(Genre ganre_is) {
    return [=](const Book &book) -> bool { return book.genre == ganre_is; };
}

auto all_of(std::invocable<const Book &> auto &&...preds) {
    return [... ps = std::move(preds)](const Book &book) { return (ps(book) && ...); };
}

auto any_of(std::invocable<const Book &> auto &&...preds) {
    return [... ps = std::move(preds)](const Book &book) { return (ps(book) || ...); };
}

template <typename BookIterator, typename Filter>
std::vector<std::reference_wrapper<Book>> filterBooks(BookIterator first, BookIterator last, Filter filter) {
    std::vector<std::reference_wrapper<Book>> result;

    for (auto it = first; it != last; ++it) {
        if (filter(*it)) {
            result.emplace_back(*it);
        }
    }

    return result;
}
}  // namespace bookdb