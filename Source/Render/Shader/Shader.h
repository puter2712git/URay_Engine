#pragma once

#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/PushConstantRange.h"
#include "Render/Shader/Reflection/ShaderReflection.h"

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
    ~Shader();

public:
    const std::vector<uint8>& GetVertexShaderCode() const { return vertexShaderCode; }
    const std::vector<uint8>& GetFragmentShaderCode() const { return fragmentShaderCode; }

    const ShaderReflection& GetVertexReflection() const { return vertexReflection; }
    const ShaderReflection& GetFragmentReflection() const { return fragmentReflection; }
    const ShaderReflection& GetMergedReflection() const { return mergedReflection; }

    const std::map<uint32, DescriptorSetLayoutDesc>& GetLayoutDescriptions() const { return layoutDescriptions; }
    const DescriptorSetLayoutDesc* GetLayoutDescription(uint32 set) const;

    const std::vector<PushConstantRange>& GetPushConstantRanges() const { return pushConstantRanges; }

private:
    // Merge Reflections
    void MergeReflection();

    void MergeDescriptorBindings();
    void MergeUniformBuffers();
    void MergePushConstant();

    ShaderDescriptorBinding* FindBinding(uint32 set, uint32 binding);
    ShaderUniformBuffer* FindUniformBuffer(uint32 set, uint32 binding);

    // Create Descriptor Set Layout Descriptions
    void CreateSetLayoutDescriptions();

private:
    std::vector<uint8> vertexShaderCode;
    std::vector<uint8> fragmentShaderCode;

    ShaderReflection vertexReflection = {};
    ShaderReflection fragmentReflection = {};
    ShaderReflection mergedReflection = {};

    std::map<uint32, DescriptorSetLayoutDesc> layoutDescriptions;
    std::vector<PushConstantRange> pushConstantRanges;
};

} // namespace URay::Render
