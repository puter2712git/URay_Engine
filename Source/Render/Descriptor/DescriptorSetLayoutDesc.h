#pragma once

#include "ResourceBinding.h"

#include <vector>

namespace URay
{

struct DescriptorSetLayoutDesc
{
    std::vector<ResourceBinding> bindings;

    bool operator==(const DescriptorSetLayoutDesc&) const = default;
};

struct DescriptorSetLayoutDescHash
{
    size_t operator()(const DescriptorSetLayoutDesc& desc) const
    {
        size_t hash = 0;

        auto combine = [&hash](auto value)
        {
            size_t valueHash = std::hash<decltype(value)>{}(value);
            hash ^= valueHash + 0x9e3779b9 + (hash << 6) + (hash << 2);
        };

        for (const auto& binding : desc.bindings)
        {
            combine(binding.bindingIndex);
            combine(binding.resourceType);
            combine(binding.arrayCount);
            combine(binding.stageFlags);
        }

        return hash;
    }
};

} // namespace URay
