#pragma once

#include <print>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    // Type aliases
    using iterator = BookContainer::iterator;
    using const_iterator = BookContainer::const_iterator;
    using size_type = BookContainer::size_type;
    using AuthorContainer = std::unordered_set<std::string>;

    BookDatabase() = default;

    BookDatabase(std::initializer_list<Book> books) : books_{books} {
        for (const auto &book : books) {
            auto [ath, tmp] = authors_.emplace(book.author);
            Book tmp_book = book;
            tmp_book.author = *ath;
            books_.push_back(std::move(tmp_book));
        }
    }

    void PushBack(const Book &book) {
        auto [ath, tmp] = authors_.emplace(book.author);
        Book tmp_book = book;
        tmp_book.author = *ath;
        books_.push_back(std::move(tmp_book));
    }

    void PushBack(const Book &&book) {
        auto [ath, tmp] = authors_.emplace(book.author);
        Book tmp_book = book;
        tmp_book.author = *ath;
        books_.push_back(std::move(tmp_book));
    }

    template <typename... Args>
    void EmplaceBack(Args &&...args) {
        PushBack(Book(std::forward<Args>(args)...));
    }

    void Clear() {
        books_.clear();
        authors_.clear();
    }

    Book &operator[](size_type idx) { return books_[idx]; }

    iterator begin() noexcept { return books_.begin(); }
    iterator end() noexcept { return books_.end(); }
    const_iterator begin() const noexcept { return books_.begin(); }
    const_iterator end() const noexcept { return books_.end(); }
    const_iterator cbegin() const noexcept { return books_.cbegin(); }
    const_iterator cend() const noexcept { return books_.cend(); }

    size_type size() const noexcept { return books_.size(); }
    bool empty() const noexcept { return books_.empty(); }

    const BookContainer &GetBooks() const noexcept { return books_; }
    const AuthorContainer &GetAuthors() const noexcept { return authors_; }

    void reserve(size_type n) { books_.reserve(n); }

private:
    BookContainer books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

template <>
struct std::formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {
        /*
        Раскомментируйте, когда bookdb::BookDatabase поддержит интерфейсы, доступные стандартным контейнерам
        (size/begin/...)
        */
        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }

        return fc.out();
    }

    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
