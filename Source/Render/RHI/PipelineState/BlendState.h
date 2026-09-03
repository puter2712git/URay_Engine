#pragma once

namespace URay::Render
{

enum class BlendMode
{
    Opaque,
    AlphaBlend
};

struct BlendState
{
    BlendMode mode = BlendMode::Opaque;

    bool operator==(const BlendState&) const = default;
};

} // namespace URay::Render
