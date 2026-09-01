#pragma once

#include "Core/Type/Types.h"

#include <cstdint>

namespace URay::Render
{

enum class Format
{
    Unknown,

    RGBA8_UNorm,
    RGBA8_sRGB,

    BGRA8_sRGB,

    D32_Float,
    D32_Float_S8_UInt,
    D24_UNorm_S8_UInt,
};

enum class TextureUsage : uint32
{
    None = 0,
    TransferSrc = 1 << 0,
    TransferDst = 1 << 1,
    Sampled = 1 << 2,
    ColorAttachment = 1 << 3,
    DepthAttachment = 1 << 4,
};

constexpr TextureUsage operator|(TextureUsage lhs, TextureUsage rhs)
{
    return static_cast<TextureUsage>(
        static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
}

constexpr TextureUsage operator&(TextureUsage lhs, TextureUsage rhs)
{
    return static_cast<TextureUsage>(
        static_cast<uint32>(lhs) & static_cast<uint32>(rhs));
}

struct TextureDesc
{
    uint32 width = 0;
    uint32 height = 0;

    Format format = Format::Unknown;
    TextureUsage usage = TextureUsage::None;
};

} // namespace URay::Render
