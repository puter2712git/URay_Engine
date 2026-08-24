#pragma once

namespace URay::Render
{

struct BlendState
{
    bool blendEnable = true;

    bool operator==(const BlendState&) const = default;
};

} // namespace URay::Render
