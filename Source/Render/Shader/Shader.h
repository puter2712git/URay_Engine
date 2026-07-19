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
    uint64_t GetId() const
    {
        return id;
    }
    void SetId(uint64_t newId)
    {
        id = newId;
    }

    const std::string& GetFilePath() const
    {
        return filePath;
    }

    const std::vector<char>& GetCode() const
    {
        return code;
    }

private:
    uint64_t id = 0;
    std::string filePath;
    std::vector<char> code;

    VkShaderStageFlagBits stage = VK_SHADER_STAGE_VERTEX_BIT;
    std::string entry;
};

} // namespace URay
