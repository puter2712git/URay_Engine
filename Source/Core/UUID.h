#pragma once

#include "Core/Type/Types.h"

#include <cstdint>
#include <string>

namespace URay
{

struct UUID
{
    uint64 high;
    uint64 low;

    static UUID Generate();

    std::string ToString() const;
    static UUID FromString(const std::string& str);

    bool IsValid() const { return high != 0 || low != 0; }

    auto operator<=>(const UUID&) const = default;
};

struct UUIDHash
{
    size_t operator()(const UUID& uuid) const noexcept
    {
        const size_t highHash = std::hash<uint64>{}(uuid.high);
        const size_t lowHash = std::hash<uint64>{}(uuid.low);

        return highHash ^ (lowHash + 0x9e3779b9 + (highHash << 6) + (highHash >> 2));
    }
};

} // namespace URay
