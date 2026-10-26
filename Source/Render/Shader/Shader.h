#pragma once
#include <Core/Type/TypeAlias.h>

#include <Render/RHI/Descriptor/DescriptorSetLayoutDesc.h>
#include <Render/RHI/PushConstantRange.h>
#include <Render/Shader/Reflection/ShaderReflection.h>

#include <map>
#include <memory>
#include <vector>

namespace URay::Render
{

struct ShaderStageData
{
    ShaderStageFlags stage = ShaderStageFlags::None;
    String entryPoint;
    std::vector<uint8> code;
    ShaderReflection reflection = {};
};

class Shader
{
public:
    Shader(std::vector<ShaderStageData> stages);
    ~Shader();

public:
    bool HasStage(ShaderStageFlags stage) const;

    const ShaderStageData* GetStageData(ShaderStageFlags stage) const;

    bool IsGraphics() const;
    bool IsCompute() const;

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
    std::unique_ptr<ShaderStageData> vertexStageData = nullptr;
    std::unique_ptr<ShaderStageData> fragmentStageData = nullptr;
    std::unique_ptr<ShaderStageData> computeStageData = nullptr;

    ShaderReflection mergedReflection = {};

    std::map<uint32, DescriptorSetLayoutDesc> layoutDescriptions;
    std::vector<PushConstantRange> pushConstantRanges;
};

} // namespace URay::Render
