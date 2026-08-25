#pragma once

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

enum class TextureUsage : uint32_t
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
        static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr TextureUsage operator&(TextureUsage lhs, TextureUsage rhs)
{
    return static_cast<TextureUsage>(
        static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

struct TextureDesc
{
    uint32_t width = 0;
    uint32_t height = 0;

    Format format = Format::Unknown;
    TextureUsage usage = TextureUsage::None;
};

} // namespace URay::Render
