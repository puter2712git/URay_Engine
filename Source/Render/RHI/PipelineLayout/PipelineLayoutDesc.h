#pragma once

#include "Render/RHI/PushConstantRange.h"

#include "Core/Type/Types.h"

#include <map>
#include <vector>

namespace URay::Render
{

class DescriptorSetLayout;

struct PipelineLayoutDesc
{
    std::map<uint32, DescriptorSetLayout*> setLayouts;
    std::vector<PushConstantRange> pushConstantRanges;

    bool operator==(const PipelineLayoutDesc&) const = default;
};

struct PipelineLayoutDescHash
{
    size_t operator()(const PipelineLayoutDesc& desc) const
    {
        size_t hash = 0;

        auto combine = [&hash](auto value)
        {
            const size_t valueHash = std::hash<decltype(value)>{}(value);
            hash ^= valueHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        };

        combine(desc.setLayouts.size());
        for (const auto& [set, layout] : desc.setLayouts)
        {
            combine(set);
            combine(layout);
        }

        combine(desc.pushConstantRanges.size());
        for (const PushConstantRange& range : desc.pushConstantRanges)
        {
            combine(range.offset);
            combine(range.size);
            combine(range.stages);
        }

        return hash;
    }
};

} // namespace URay::Render
