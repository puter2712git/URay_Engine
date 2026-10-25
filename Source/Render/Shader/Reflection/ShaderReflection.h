#pragma once
#include <Core/Type/Types.h>

#include <Render/RHI/Descriptor/ResourceBinding.h>
#include <Render/Shader/ShaderStageFlags.h>


#include <vulkan/vulkan.h>

#include <vector>

namespace URay::Render
{

struct ShaderVertexInput
{
    String name;
    String semantic;

    uint32 location = 0;
    uint32 component = 0;

    VkFormat format = VK_FORMAT_UNDEFINED;
};

struct ShaderDescriptorBinding
{
    String name;

    uint32 set = 0;
    uint32 binding = 0;

    ResourceType type = ResourceType::Sampler;
    uint32 count = 0;

    ShaderStageFlags stageFlags = ShaderStageFlags::None;
};

struct ShaderParameter
{
    String name;

    uint32 offset = 0;
    uint32 size = 0;

    uint32 arrayStride = 0;
    uint32 matrixStride = 0;

    bool operator==(const ShaderParameter&) const = default;
};

struct ShaderUniformBuffer
{
    String name;

    uint32 set = 0;
    uint32 binding = 0;

    uint32 size = 0;

    ShaderStageFlags stageFlags = ShaderStageFlags::None;

    std::vector<ShaderParameter> parameters;
};

struct ShaderPushConstant
{
    uint32 offset = 0;
    uint32 size = 0;

    ShaderStageFlags stageFlags = ShaderStageFlags::None;

    std::vector<ShaderParameter> parameters;
};

struct ShaderReflection
{
    std::vector<ShaderVertexInput> vertexInputs;
    std::vector<ShaderDescriptorBinding> descriptorBindings;
    std::vector<ShaderUniformBuffer> uniformBuffers;
    ShaderPushConstant pushConstant = {};
};

} // namespace URay::Render
