#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace URay
{

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using String = std::string;
using StringView = std::string_view;

template <typename T, std::size_t N>
using Array = std::array<T, N>;

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using Span = std::span<T>;

template <typename K, typename V, typename Hash = std::hash<K>>
using HashMap = std::unordered_map<K, V, Hash>;

template <typename T>
using UniquePtr = std::unique_ptr<T>;

template <typename T, typename... Args>
UniquePtr<T> MakeUnique(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

} // namespace URay
