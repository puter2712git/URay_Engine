#pragma once

#include "Render/Shader/Reflection/ShaderReflection.h"
#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/PushConstantRange.h"

#include "Core/Type/Types.h"

#include <map>
#include <vector>

namespace URay::Render
{

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
    const ShaderPipelineReflection& GetPipelineReflection() const { return pipelineReflection; }

private:
    std::vector<uint8> vertexShaderCode;
    std::vector<uint8> fragmentShaderCode;

    ShaderReflection vertexReflection = {};
    ShaderReflection fragmentReflection = {};
    ShaderPipelineReflection pipelineReflection = {};

    std::map<uint32, DescriptorSetLayoutDesc> setLayoutDescs;
    std::vector<PushConstantRange> pushConstantRanges;
};

} // namespace URay::Render
