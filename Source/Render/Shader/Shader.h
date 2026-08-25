#pragma once

#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/PushConstantRange.h"

#include <vulkan/vulkan.h>

#include <map>
#include <string>
#include <vector>

namespace URay::Render
{

struct ShaderStage
{
    std::string filePath;
    std::vector<uint8_t> code;
    VkShaderStageFlags stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    std::string entry;
};

struct ShaderReflection
{
    std::map<uint32_t, DescriptorSetLayoutDesc> setLayoutDescs = {};
    PushConstantRange pushConstantRange = {};
};

class Shader
{
public:
    Shader(uint32_t id,
           const ShaderStage& vertexStage,
           const ShaderStage& fragmentStage,
           const ShaderReflection& vertexReflection,
           const ShaderReflection& fragmentReflection);
    ~Shader() = default;

public:
    const std::map<uint32_t, DescriptorSetLayoutDesc>& GetDescriptorSetLayoutDescs() const { return setLayoutDescs; }
    const DescriptorSetLayoutDesc* GetDescriptorSetLayoutDesc(uint32_t set) const;

    const std::vector<PushConstantRange>& GetPushConstantRanges() const { return pushConstantRanges; }

    uint64_t GetId() const { return id; }

    const ShaderStage& GetVertexStage() const { return vertexStage; }
    const ShaderStage& GetFragmentStage() const { return fragmentStage; }

    const ShaderReflection& GetVertexReflection() const { return vertexReflection; }
    const ShaderReflection& GetFragmentReflection() const { return fragmentReflection; }

private:
    uint64_t id = 0;

    ShaderStage vertexStage = {};
    ShaderStage fragmentStage = {};

    ShaderReflection vertexReflection = {};
    ShaderReflection fragmentReflection = {};

    std::map<uint32_t, DescriptorSetLayoutDesc> setLayoutDescs;
    std::vector<PushConstantRange> pushConstantRanges;
};

} // namespace URay::Render
