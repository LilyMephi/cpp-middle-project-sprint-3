#pragma once

#include <fmt/format.h>
#include <stdexcept>
#include <string_view>
#include <array>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

// Ваш код для constexpr преобразования строк в enum::Genre и наоборот здесь

constexpr Genre GenreFromString(std::string_view s) {
    constexpr std::array data = std::to_array<std::pair<std::string_view, Genre>>({
        { "Fiction", Genre::Fiction },
        { "NonFiction", Genre::NonFiction },
        { "SciFi", Genre::SciFi },
        { "Biography", Genre::Biography },
        { "Mystery", Genre::Mystery }
    });
    
    const auto it = std::find_if(data.begin(), data.end(), 
        [s](const auto& p) { return p.first == s; });
    
    return it != data.end() ? it->second : Genre::Unknown;
}

struct Book {
    std::string author;
    std::string title;

    int year;
    Genre genre;
    double rating;
    int read_count;

    Book(std::string_view ttl, std::string auth, int yr, Genre g, double rt, int rc)
    :  title{ttl}, author{auth}, year(yr), genre(g), rating(rt), read_count(rc)
    {
    }

    Book(std::string_view ttl, std::string auth, int yr, std::string_view g, double rt, int rc)
    :  title{ttl}, author{auth}, year(yr), genre(GenreFromString(g)), rating(rt), read_count(rc)
    {
    }

    auto operator<=>(const Book&) const = default;
};
}  // namespace bookdb

template <>
struct fmt::formatter<bookdb::Genre, char> {
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

    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

template <>
struct fmt::formatter<bookdb::Book, char> {
    template <typename FormatContext>
    auto format(const bookdb::Book& b, FormatContext& fc) const {
        return format_to(fc.out(), 
            "Book(title=\"{}\", author=\"{}\", year={}, genre={}, rating={:.1f}, read_count={})",
            b.title, b.author, b.year, b.genre, b.rating, b.read_count);
    }

    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

