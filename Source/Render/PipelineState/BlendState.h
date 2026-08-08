#pragma once

namespace URay
{

struct BlendState
{
    bool blendEnable = true;

    bool operator==(const BlendState&) const = default;
};

} // namespace URay
