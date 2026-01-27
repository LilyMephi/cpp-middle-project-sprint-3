#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
  bool operator()(const Book& b_f, const Book& b_s) const {
    return b_f.author < b_s.author;
  }
};

struct LessByPopularity{
  bool operator()(const Book& b_f, const Book& b_s) const {
    return b_f.read_count < b_s.read_count;
  }
};

struct LessByRating{
  bool operator()(const Book& b_f, const Book& b_s) const {
    return b_f.rating < b_s.rating;
  }
};

}  // namespace bookdb::comp