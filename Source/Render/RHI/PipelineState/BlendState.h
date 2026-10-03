#pragma once

#include "Core/Type/Types.h"

namespace URay::Render
{

enum class BlendMode
{
    Opaque,
    AlphaBlend
};

enum class ColorWriteMask : uint8
{
    None = 0,
    R = 1 << 0,
    G = 1 << 1,
    B = 1 << 2,
    A = 1 << 3,
    RGBA = R | G | B | A
};

constexpr ColorWriteMask operator|(ColorWriteMask lhs, ColorWriteMask rhs)
{
    return static_cast<ColorWriteMask>(static_cast<uint8>(lhs) | static_cast<uint8>(rhs));
}

struct ColorBlendAttachmentState
{
    BlendMode mode = BlendMode::Opaque;
    ColorWriteMask writeMask = ColorWriteMask::RGBA;

    bool operator==(const ColorBlendAttachmentState&) const = default;
};

} // namespace URay::Render
