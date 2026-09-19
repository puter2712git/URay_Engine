#pragma once

#include "Core/Type/Types.h"

#include <vulkan/vulkan.h>

#include <functional>

namespace URay::Render
{

class Texture;

enum class TextureViewType : uint8
{
    Texture1D,
    Texture2D,
    Texture2DArray,
    Texture3D,
    TextureCube,
    TextureCubeArray
};

struct TextureViewDesc
{
    TextureViewType type = TextureViewType::Texture2D;
    uint32 baseMipLevel = 0;
    uint32 mipLevelCount = 1;
    uint32 baseArrayLayer = 0;
    uint32 arrayLayerCount = 1;

    bool operator==(const TextureViewDesc&) const = default;
};

struct TextureViewDescHash
{
    size_t operator()(const TextureViewDesc& desc) const noexcept
    {
        size_t hash = 0;

        auto combine = [&hash](uint32 value)
        {
            const size_t valueHash = std::hash<uint32>{}(value);
            hash ^= valueHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        };

        combine(static_cast<uint32>(desc.type));
        combine(desc.baseMipLevel);
        combine(desc.mipLevelCount);
        combine(desc.baseArrayLayer);
        combine(desc.arrayLayerCount);
        return hash;
    }
};

struct TextureViewKey
{
    Texture* texture = nullptr;
    TextureViewDesc desc = {};

    bool operator==(const TextureViewKey&) const = default;
};

struct TextureViewKeyHash
{
    size_t operator()(const TextureViewKey& key) const noexcept
    {
        size_t hash = std::hash<Texture*>{}(key.texture);
        const size_t descHash = TextureViewDescHash{}(key.desc);

        hash ^= descHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

class TextureView
{
public:
    TextureView(VkDevice device, VkImageView handle, Texture* ref, const TextureViewDesc& viewDesc);
    ~TextureView();

public:
    VkImageView GetHandle() const { return handle; }

private:
    VkDevice device = VK_NULL_HANDLE;

    VkImageView handle = VK_NULL_HANDLE;

    Texture* refTexture = nullptr;
    TextureViewDesc desc = {};
};

} // namespace URay::Render
