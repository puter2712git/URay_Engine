#pragma once

#include <cstdint>
#include <string>

namespace URay
{

struct UUID
{
    uint64_t high;
    uint64_t low;

    static UUID Generate();

    std::string ToString() const;
    static UUID FromString(const std::string& str);

    bool IsValid() const { return high != 0 || low != 0; }

    auto operator<=>(const UUID&) const = default;
};

} // namespace URay
