#pragma once

#include "Render/Shader/ShaderDefine.h"

#include <vector>

namespace URay
{
class Shader;
}

namespace URay::Render
{

struct ShaderPermutationKey
{
    const URay::Shader* shader = nullptr;
    std::vector<ShaderDefine> defines;

    bool operator==(const ShaderPermutationKey&) const = default;
};

struct ShaderPermutationKeyHash
{
    size_t operator()(const ShaderPermutationKey& key) const
    {
        size_t hash = 0;

        auto combine = [&hash](auto value)
        {
            const size_t valueHash = std::hash<decltype(value)>{}(value);
            hash ^= valueHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        };

        combine(key.shader);

        for (const ShaderDefine& define : key.defines)
        {
            combine(define.name);
            combine(define.value);
        }

        return hash;
    }
};

} // namespace URay::Render
