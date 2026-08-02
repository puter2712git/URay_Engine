#pragma once

#include "Render/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/PushConstantRange.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace URay
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
    DescriptorSetLayoutDesc setLayoutDesc = {};
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
};

} // namespace URay
