#pragma once

#include <string>
#include <unordered_map>

#include <vulkan/vulkan.h>

namespace URay
{

class Shader;

class ShaderManager
{
public:
    ShaderManager() = default;
    ~ShaderManager();

public:
    Shader* GetOrCreate(const std::string& key, const std::string& filePath = "",
                        VkShaderStageFlagBits stage = VK_SHADER_STAGE_VERTEX_BIT,
                        const std::string& entry = "main");

private:
    std::unordered_map<std::string, Shader*> shaders;
};

} // namespace URay
