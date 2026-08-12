#pragma once

#include <string>
#include <unordered_map>

#include <vulkan/vulkan.h>

namespace URay::RHI
{

class Shader;

class ShaderManager
{
public:
    ShaderManager() = default;
    ~ShaderManager();

public:
    Shader* GetOrCreate(const std::string& key,
                        const std::string& vertexFilePath = "",
                        const std::string& fragmentFilePath = "");

private:
    uint64_t shaderIdCounter = 0;
    std::unordered_map<std::string, Shader*> shaders;
};

} // namespace URay::RHI
