#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

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

template <typename T>
using UniquePtr = std::unique_ptr<T>;

template <typename T, typename... Args>
UniquePtr<T> MakeUnique(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

} // namespace URay
