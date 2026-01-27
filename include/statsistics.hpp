#pragma once

#include <algorithm>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>
#include <algorithm>

#include "book_database.hpp"
#include "filters.hpp"

namespace bookdb {

template <BookContainerLike T, typename BookComparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, BookComparator comp = {}) {
    using size_type = typename T::size_type;
    using AuthorContainer = typename BookDatabase<T>::AuthorContainer;
    using pair_type = std::pair<std::string_view, size_type>;
    
    std::vector<pair_type> histogram;
    histogram.reserve(cont.GetAuthors().size());

    for (const auto& [author, indices] : cont.GetAuthors()) {
        histogram.emplace_back(author, static_cast<size_type>(indices.size()));
    }

    std::sort(histogram.begin(), histogram.end(), 
              [&comp](const pair_type& a, const pair_type& b) {
                  if (a.second != b.second) {
                      return a.second > b.second;
                  }
                  return comp(a.first, b.first);
              });
    
    return histogram;
}


template <BookIterator Iterator>
auto calculateGenreRatings(Iterator first, Iterator last) {
    std::unordered_map<Genre, double> res;
    
    for (auto book = first; book != last; ++book) {
        if (!res.contains(book->genre)) {
            Genre genre = book->genre;  // Явно объявляем локальную переменную
            
            auto count = std::count_if(first, last, 
                [genre](const Book& b) { return b.genre == genre; });
            
            auto sum = std::accumulate(first, last, 0.0, 
                [genre](double acc, const Book& b) { 
                    return acc + (b.genre == genre ? b.rating : 0.0); 
                });
                
            res[book->genre] = sum / count;
        }
    }
    return res;
}

template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T>& db) {
    if (db.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& book : db) {
        sum += book.rating;
    }
    return sum / db.size();
}

template <BookContainerLike T, typename BookComparator>
std::vector<std::reference_wrapper<const Book>> getTopNBy(
   const BookDatabase<T>& books, 
    size_t count,
  BookComparator comp
  ) {
    
    std::vector<std::reference_wrapper<const Book>> refs(books.begin(), books.end());
    
    std::sort(refs.begin(), refs.end(),
        [comp](auto&& a, auto&& b) {
            return comp(b.get(), a.get());
        });
    
    refs.erase(refs.begin() + count, refs.end());
    return refs;
}


}  // namespace bookdb
