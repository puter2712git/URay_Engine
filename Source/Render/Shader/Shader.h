#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace URay
{

class Shader
{
public:
    Shader(const std::string& filePath, const std::vector<char>& code,
           VkShaderStageFlagBits stage, const std::string& entry);
    ~Shader() = default;

public:
    const std::string& GetFilePath() const
    {
        return filePath;
    }

    const std::vector<char>& GetCode() const
    {
        return code;
    }

private:
    std::string filePath;
    std::vector<char> code;

    VkShaderStageFlagBits stage = VK_SHADER_STAGE_VERTEX_BIT;
    std::string entry;
};

} // namespace URay
