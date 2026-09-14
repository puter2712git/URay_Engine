#pragma once

#include "Render/RHI/Descriptor/ResourceBinding.h"
#include "Render/Shader/ShaderStageFlags.h"

#include "Core/Type/Types.h"

#include <string>
#include <vector>

namespace URay::Render
{

struct ShaderDescriptorBinding
{
    std::string name;

    uint32 set = 0;
    uint32 binding = 0;

    ResourceType type = ResourceType::Sampler;
    uint32 count = 0;

    ShaderStageFlags stageFlags = ShaderStageFlags::None;
};

struct ShaderParameter
{
    std::string name;

    uint32 offset = 0;
    uint32 size = 0;

    uint32 arrayStride = 0;
    uint32 matrixStride = 0;
};

struct ShaderUniformBuffer
{
    std::string name;

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

    std::vector<ShaderParameter> parameters;
};

struct ShaderReflection
{
    std::vector<ShaderDescriptorBinding> descriptorBindings;
    std::vector<ShaderUniformBuffer> uniformBuffers;
    ShaderPushConstant pushConstant = {};
};

} // namespace URay::Render
