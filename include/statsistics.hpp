#pragma once

#include <algorithm>
#include <execution>
#include <flat_map>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>

#include "book_database.hpp"
#include "filters.hpp"

namespace bookdb {

template <BookContainerLike T, typename BookComparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &books, BookComparator comp = {}) {
    using size_type = typename T::size_type;
    using AuthorContainer = typename BookDatabase<T>::AuthorContainer;
    using pair_type = std::pair<std::string_view, size_type>;

    std::flat_map<std::string_view, size_type, BookComparator> histogram;

    for (const auto &author : books.GetAuthors()) {
        size_type count = std::count_if(books.GetBooks().begin(), books.GetBooks().end(),
                                        [author](const Book &b) { return b.author == author; });
        histogram[author] = count;
    }

    return histogram;
}

template <BookIterator Iterator>
auto calculateGenreRatings(const Iterator first, const Iterator last) {
    std::flat_map<Genre, double> res;

    for (auto book = first; book != last; ++book) {
        if (!res.contains(book->genre)) {
            Genre genre = book->genre;

            auto count = std::count_if(first, last, [genre](const Book &b) { return b.genre == genre; });

            auto sum = std::transform_reduce(std::execution::par, first, last, 0.0, std::plus<>{},
                                             [genre](const Book &b) { return (b.genre == genre ? b.rating : 0.0); });
            if (count > 0) {
                res[genre] = sum / static_cast<double>(count);
            }
        }
    }
    return res;
}

template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &db) {
    if (db.empty())
        return 0.0;

    double sum = 0.0;
    for (const auto &book : db) {
        sum += book.rating;
    }
    return sum / db.size();
}

template <BookContainerLike T, typename BookComparator>
std::vector<std::reference_wrapper<const Book>> getTopNBy(const BookDatabase<T> &books, size_t count,
                                                          BookComparator comp) {

    std::vector<std::reference_wrapper<const Book>> refs(books.begin(), books.end());

    std::sort(refs.begin(), refs.end(), [comp](auto &&a, auto &&b) { return comp(b.get(), a.get()); });

    refs.erase(refs.begin() + count, refs.end());
    return refs;
}

}  // namespace bookdb
