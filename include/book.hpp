#pragma once

#include <array>
#include <format>
#include <stdexcept>
#include <string_view>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

// Ваш код для constexpr преобразования строк в enum::Genre и наоборот здесь

constexpr Genre GenreFromString(std::string_view s) {
    constexpr std::array data = std::to_array<std::pair<std::string_view, Genre>>({{"Fiction", Genre::Fiction},
                                                                                   {"NonFiction", Genre::NonFiction},
                                                                                   {"SciFi", Genre::SciFi},
                                                                                   {"Biography", Genre::Biography},
                                                                                   {"Mystery", Genre::Mystery}});

    const auto it = std::find_if(data.begin(), data.end(), [s](const auto &p) { return p.first == s; });

    return it != data.end() ? it->second : Genre::Unknown;
}

struct Book {
    std::string_view author;
    std::string title;

    int year;
    Genre genre;
    double rating;
    int read_count;

    constexpr Book(std::string_view ttl, std::string_view auth, int yr, Genre g, double rt, int rc) noexcept
        : title{ttl}, author{auth}, year(yr), genre(g), rating(rt), read_count(rc) {}

    constexpr Book(std::string_view ttl, std::string_view auth, int yr, std::string_view genre_str, double rt, int rc)
        : title{ttl}, author{auth}, year(yr), rating(rt), read_count(rc) {
        genre = GenreFromString(genre_str);
    }

    constexpr Book(std::string_view ttl, std::string auth_str, int yr, Genre g, double rt, int rc) noexcept
        : title{ttl}, author{std::move(auth_str)}, year(yr), genre(g), rating(rt), read_count(rc) {}

    constexpr Book(const char *ttl, const char *auth, int yr, Genre g, double rt, int rc) noexcept
        : title{ttl}, author{auth}, year(yr), genre(g), rating(rt), read_count(rc) {}

    auto operator<=>(const Book &) const = default;
};
}  // namespace bookdb

template <>
struct std::formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        std::string genre_str;

        // clang-format off
        using bookdb::Genre;
        switch (g) {
            case Genre::Fiction:    genre_str = "Fiction"; break;
            case Genre::Mystery:    genre_str = "Mystery"; break;
            case Genre::NonFiction: genre_str = "NonFiction"; break;
            case Genre::SciFi:      genre_str = "SciFi"; break;
            case Genre::Biography:  genre_str = "Biography"; break;
            case Genre::Unknown:    genre_str = "Unknown"; break;
            default:
                throw std::logic_error{"Unsupported bookdb::Genre"};
            }
        // clang-format on
        return format_to(fc.out(), "{}", genre_str);
    }

    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

template <>
struct std::formatter<bookdb::Book, char> {
    template <typename FormatContext>
    auto format(const bookdb::Book &b, FormatContext &fc) const {
        return format_to(fc.out(), "Book(title=\"{}\", author=\"{}\", year={}, genre={}, rating={:.1f}, read_count={})",
                         b.title, b.author, b.year, b.genre, b.rating, b.read_count);
    }

    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
