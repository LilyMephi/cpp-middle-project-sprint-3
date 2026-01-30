#pragma once

#include <string>
#include <string_view>

namespace bookdb {

struct TransparentStringLess {
    using is_transparent = void;

    bool operator()(std::string_view lhs, std::string_view rhs) const { return lhs < rhs; }

    bool operator()(const std::string &lhs, std::string_view rhs) const { return lhs < rhs; }

    bool operator()(std::string_view lhs, const std::string &rhs) const { return lhs < rhs; }

    bool operator()(const std::string lhs, const std::string &rhs) const { return lhs < rhs; }
};

struct TransparentStringEqual {
    using is_transparent = void;

    bool operator()(std::string_view lhs, std::string_view rhs) const { return lhs == rhs; }

    bool operator()(const std::string &lhs, std::string_view rhs) const { return lhs == rhs; }

    bool operator()(std::string_view lhs, const std::string &rhs) const { return lhs == rhs; }
};

struct TransparentStringHash {
    using is_transparent = void;

    size_t operator()(std::string_view key) const { return std::hash<std::string_view>{}(key); }

    size_t operator()(const std::string &key) const { return std::hash<std::string>{}(key); }
};

}  // namespace bookdb
