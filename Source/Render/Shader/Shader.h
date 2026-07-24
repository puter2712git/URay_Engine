#pragma once

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

class Shader
{
public:
    Shader(const ShaderStage& vertexStage, const ShaderStage& fragmentStage);
    ~Shader() = default;

public:
    uint64_t GetId() const
    {
        return id;
    }
    void SetId(uint64_t newId)
    {
        id = newId;
    }

    const ShaderStage& GetVertexStage() const
    {
        return vertexStage;
    }
    const ShaderStage& GetFragmentStage() const
    {
        return fragmentStage;
    }

private:
    uint64_t id = 0;

    ShaderStage vertexStage = {};
    ShaderStage fragmentStage = {};
};

} // namespace URay
