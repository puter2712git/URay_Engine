#pragma once
#include <Core/Type/TypeAlias.h>

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

enum class ShaderParameterType
{
    Unsupported,
    Float,
    Float2,
    Float3,
    Float4
};

struct ShaderParameter
{
    String name;

    ShaderParameterType type = ShaderParameterType::Unsupported;

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

    Vector<ShaderParameter> parameters;
};

struct ShaderPushConstant
{
    uint32 offset = 0;
    uint32 size = 0;

    ShaderStageFlags stageFlags = ShaderStageFlags::None;

    Vector<ShaderParameter> parameters;
};

struct ShaderReflection
{
    Vector<ShaderVertexInput> vertexInputs;
    Vector<ShaderDescriptorBinding> descriptorBindings;
    Vector<ShaderUniformBuffer> uniformBuffers;
    ShaderPushConstant pushConstant = {};
};

} // namespace URay::Render
