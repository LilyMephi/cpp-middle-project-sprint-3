#include <gtest/gtest.h>
#include <unordered_map>

#include "book.hpp"
#include "book_database.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "statsistics.hpp"

TEST(GenreFromStringTest, ValidGenres) {
    EXPECT_EQ(bookdb::GenreFromString("Fiction"), bookdb::Genre::Fiction);
    EXPECT_EQ(bookdb::GenreFromString("NonFiction"), bookdb::Genre::NonFiction);
    EXPECT_EQ(bookdb::GenreFromString("SciFi"), bookdb::Genre::SciFi);
    EXPECT_EQ(bookdb::GenreFromString("Biography"), bookdb::Genre::Biography);
    EXPECT_EQ(bookdb::GenreFromString("Mystery"), bookdb::Genre::Mystery);
}

TEST(BookFormatterTest, BasicFormatting) {
    bookdb::Book sci_fi_book{"1984", "George Orwell", 1949, bookdb::Genre::SciFi, 4.65, 3};
    std::string result = std::format("{}", sci_fi_book);
    EXPECT_EQ(result,
              "Book(title=\"1984\", author=\"George Orwell\", year=1949, genre=SciFi, rating=4.7, read_count=3)");
}

TEST(BookFormatterTest, EmptyFields) {
    bookdb::Book empty_title{"", "Author", 2000, "Fiction", 3.5, 1};
    std::string result = std::format("{}", empty_title);
    EXPECT_EQ(result, "Book(title=\"\", author=\"Author\", year=2000, genre=Fiction, rating=3.5, read_count=1)");
}

struct BookDatabaseTest : public ::testing::Test {
    bookdb::BookDatabase<> db;  // используем default std::vector<Book>
};

// Тест конструктора и базовых операций
TEST_F(BookDatabaseTest, DefaultConstructor) {
    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);
    EXPECT_EQ(db.GetBooks().size(), 0);
    EXPECT_EQ(db.GetAuthors().size(), 0);
}

TEST_F(BookDatabaseTest, PushBackSingleBook) {
    bookdb::Book book1{"1984", "George Orwell", 1949, bookdb::Genre::SciFi, 4.65, 300};
    db.PushBack(std::move(book1));

    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db[0].title, "1984");
    EXPECT_EQ(db[0].author, "George Orwell");
}

TEST_F(BookDatabaseTest, EmplaceBackCreatesBook) {
    bookdb::BookDatabase<> db1;
    db.EmplaceBack("Animal Farm", "George Orwell", 1960, bookdb::Genre::SciFi, 5.45, 100);
    db.EmplaceBack("1984", "George Orwell", 1949, bookdb::Genre::SciFi, 4.65, 300);

    EXPECT_EQ(db.size(), 2);
    EXPECT_EQ(db[0].title, "Animal Farm");
}

TEST(BookDatabase, EmplaceBackMultipleParams) {
    bookdb::BookDatabase<> data;
    data.EmplaceBack("Animal Farm", "George Orwell", 1960, bookdb::Genre::SciFi, 5.45, 100);
    data.EmplaceBack("1984", "George Orwell", 1949, bookdb::Genre::SciFi, 4.65, 300);

    ASSERT_EQ(2u, data.size());

    const auto &first = data[0];
    EXPECT_EQ("Animal Farm", first.title);
    EXPECT_EQ("George Orwell", first.author);
    EXPECT_EQ(1960, first.year);
    EXPECT_EQ(bookdb::Genre::SciFi, first.genre);
    EXPECT_DOUBLE_EQ(5.45, first.rating);
    EXPECT_EQ(100, first.read_count);

    const auto &second = data[1];
    EXPECT_EQ("1984", second.title);
    EXPECT_EQ("George Orwell", second.author);
    EXPECT_EQ(1949, second.year);
    EXPECT_EQ(bookdb::Genre::SciFi, second.genre);
    EXPECT_DOUBLE_EQ(4.65, second.rating);
    EXPECT_EQ(300, second.read_count);

    const auto &authors = data.GetAuthors();
    ASSERT_TRUE(authors.contains("George Orwell"));
    EXPECT_EQ(1, authors.count("George Orwell"));
}

using namespace bookdb;

class BookDatabaseTesting : public ::testing::Test {
protected:
    void SetUp() override {
        // Populate test database with books
        db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.0, 190);
        db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 143);
        db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.5, 120);
        db.EmplaceBack("To Kill a Mockingbird", "Harper Lee", 1960, Genre::Fiction, 4.8, 156);
        db.EmplaceBack("Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.7, 178);
        db.EmplaceBack("The Catcher in the Rye", "J.D. Salinger", 1951, Genre::Fiction, 4.3, 112);
        db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.5, 98);
        db.EmplaceBack("Jane Eyre", "Charlotte Brontë", 1847, Genre::Fiction, 4.6, 110);
        db.EmplaceBack("The Hobbit", "J.R.R. Tolkien", 1937, Genre::Fiction, 4.9, 203);
        db.EmplaceBack("Lord of the Flies", "William Golding", 1954, Genre::Fiction, 4.2, 89);
    }

    BookDatabase<std::vector<Book>> db;
};

TEST_F(BookDatabaseTesting, SortByPopularity) {
    std::sort(db.begin(), db.end(), comp::LessByPopularity{});

    EXPECT_EQ("Lord of the Flies", db[0].title);
    EXPECT_EQ("Brave New World", db[1].title);
    EXPECT_NEAR(4.2, db[0].rating, 0.01);
}

TEST_F(BookDatabaseTesting, AuthorHistogram) {
    auto histogram = buildAuthorHistogramFlat(db);

    EXPECT_EQ(9, histogram.size());
    auto it = std::find_if(
        histogram.begin(), histogram.end(),
        [](const auto &pair) { return pair.first == "George Orwell"; });
    ASSERT_NE(histogram.end(), it);
    EXPECT_EQ(2, it->second);
}

TEST_F(BookDatabaseTesting, GenreRatings) {
    auto genreRatings = calculateGenreRatings(db.begin(), db.end());

    EXPECT_TRUE(genreRatings.contains(Genre::SciFi));
    EXPECT_TRUE(genreRatings.contains(Genre::Fiction));

    EXPECT_NEAR(4.25, genreRatings[Genre::SciFi], 0.01);

    EXPECT_NEAR(4.56, genreRatings[Genre::Fiction], 0.01);
}

TEST_F(BookDatabaseTesting, AverageRating) {
    auto avgRating = calculateAverageRating(db);
    EXPECT_NEAR(4.49, avgRating, 0.01);
}

TEST_F(BookDatabaseTesting, FilterBooks) {
    auto filtered = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 1999), RatingAbove(4.5)));

    EXPECT_EQ(2, std::distance(filtered.cbegin(), filtered.cend()));
}

TEST_F(BookDatabaseTesting, TopNBooks) {
    auto topBooks = getTopNBy(db, 3, comp::LessByRating{});

    ASSERT_EQ(3, std::distance(topBooks.cbegin(), topBooks.cend()));
    EXPECT_EQ("The Hobbit", topBooks[0].get().title);             // 4.9
    EXPECT_EQ("To Kill a Mockingbird", topBooks[1].get().title);  // 4.8
    EXPECT_EQ("Pride and Prejudice", topBooks[2].get().title);    // 4.7
}

TEST_F(BookDatabaseTesting, FindByAuthor) {
    auto orwellBookIt = std::find_if(db.begin(), db.end(), [](const auto &v) { return v.author == "George Orwell"; });

    ASSERT_NE(db.end(), orwellBookIt);
    EXPECT_TRUE(orwellBookIt->title == "1984");
}
