#pragma once

#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/PushConstantRange.h"

#include "Core/Type/Types.h"

#include <map>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace URay::Render
{

struct ReflectedBlockMember
{
    std::string name;
    uint32 offset = 0;
    uint32 size = 0;
};

struct ReflectedBinding
{
    std::string name;
    uint32 set = 0;
    uint32 binding = 0;
    ResourceType resourceType = ResourceType::ConstantBuffer;
    uint32 arrayCount = 1;
    ShaderStageFlags flags = ShaderStageFlags::All;

    // For constant buffer
    uint32 blockSize = 0;
    std::vector<ReflectedBlockMember> members;
};

struct ShaderReflection
{
    std::map<uint32, DescriptorSetLayoutDesc> setLayoutDescs = {};
    std::vector<ReflectedBinding> bindings;
    PushConstantRange pushConstantRange = {};
};

class Shader
{
public:
    Shader(const std::vector<uint8>& vertexShaderCode,
           const std::vector<uint8>& fragmentShaderCode,
           const ShaderReflection& vertexReflection,
           const ShaderReflection& fragmentReflection);
    ~Shader() = default;

public:
    const std::map<uint32, DescriptorSetLayoutDesc>& GetDescriptorSetLayoutDescs() const { return setLayoutDescs; }
    const DescriptorSetLayoutDesc* GetDescriptorSetLayoutDesc(uint32 set) const;

    const std::vector<PushConstantRange>& GetPushConstantRanges() const { return pushConstantRanges; }

    const std::vector<uint8>& GetVertexShaderCode() const { return vertexShaderCode; }
    const std::vector<uint8>& GetFragmentShaderCode() const { return fragmentShaderCode; }

    const ShaderReflection& GetVertexReflection() const { return vertexReflection; }
    const ShaderReflection& GetFragmentReflection() const { return fragmentReflection; }

private:
    std::vector<uint8> vertexShaderCode;
    std::vector<uint8> fragmentShaderCode;

    ShaderReflection vertexReflection = {};
    ShaderReflection fragmentReflection = {};

    std::map<uint32, DescriptorSetLayoutDesc> setLayoutDescs;
    std::vector<PushConstantRange> pushConstantRanges;
};

} // namespace URay::Render
