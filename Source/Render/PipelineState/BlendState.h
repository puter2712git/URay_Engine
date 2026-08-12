#pragma once

namespace URay::RHI
{

struct BlendState
{
    bool blendEnable = true;

    bool operator==(const BlendState&) const = default;
};

} // namespace URay::RHI
