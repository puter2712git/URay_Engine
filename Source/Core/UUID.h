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

} // namespace URay
