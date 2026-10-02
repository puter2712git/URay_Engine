#pragma once

#include "Core/Type/Types.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace URay::Render
{

struct VertexInputBinding
{
    uint32 binding = 0;
    uint32 stride = 0;
    VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    bool operator==(const VertexInputBinding&) const = default;
};

struct VertexInputAttribute
{
    uint32 location = 0;
    uint32 binding = 0;
    VkFormat format = VK_FORMAT_UNDEFINED;
    uint32 offset = 0;

    bool operator==(const VertexInputAttribute&) const = default;
};

struct VertexInputLayout
{
    std::vector<VertexInputBinding> bindings;
    std::vector<VertexInputAttribute> attributes;

    void AddBinding(uint32 binding, uint32 stride, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX)
    {
        VertexInputBinding newBinding = {};
        newBinding.binding = binding;
        newBinding.stride = stride;
        newBinding.inputRate = inputRate;

        bindings.push_back(newBinding);
    }

    void AddAttribute(uint32 location, uint32 binding, VkFormat format, uint32 offset)
    {
        VertexInputAttribute newAttribute = {};
        newAttribute.location = location;
        newAttribute.binding = binding;
        newAttribute.format = format;
        newAttribute.offset = offset;

        attributes.push_back(newAttribute);
    }

    bool operator==(const VertexInputLayout&) const = default;
};

inline const VertexInputLayout& GetEmptyVertexInputLayout()
{
    static const VertexInputLayout layout = {};
    return layout;
}

} // namespace URay::Render
