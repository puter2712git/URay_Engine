#include "ShaderManager.h"

#include "Shader.h"
#include "ShaderReflector.h"

#include "Core/File/VirtualFilesystem.h"

namespace URay::RHI
{

ShaderManager::ShaderManager(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
}

ShaderManager::~ShaderManager()
{
    for (auto& [key, shader] : shaders)
    {
        if (shader)
        {
            delete shader;
            shader = nullptr;
        }
    }

    shaders.clear();
}

Shader* ShaderManager::GetOrCreate(const std::string& key,
                                   const VirtualPath& vertexFilePath,
                                   const VirtualPath& fragmentFilePath)
{
    auto it = shaders.find(key);
    if (it != shaders.end())
        return it->second;

    std::vector<uint8_t> vertexShaderCode = filesystem.ReadBinary(vertexFilePath);
    if (vertexShaderCode.empty())
        return nullptr;

    std::vector<uint8_t> fragmentShaderCode = filesystem.ReadBinary(fragmentFilePath);
    if (fragmentShaderCode.empty())
        return nullptr;

    ShaderReflection vertexReflection = {};
    if (!ShaderReflector::ReflectSPIRV(vertexShaderCode, vertexReflection))
        return nullptr;

    ShaderReflection fragmentReflection = {};
    if (!ShaderReflector::ReflectSPIRV(fragmentShaderCode, fragmentReflection))
        return nullptr;

    ShaderStage vertexStage = {};
    vertexStage.filePath = vertexFilePath.ToString();
    vertexStage.code = vertexShaderCode;
    vertexStage.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStage.entry = "VSMain";

    ShaderStage fragmentStage = {};
    fragmentStage.filePath = fragmentFilePath.ToString();
    fragmentStage.code = fragmentShaderCode;
    fragmentStage.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStage.entry = "PSMain";

    Shader* newShader = new Shader(
        shaderIdCounter++,
        vertexStage,
        fragmentStage,
        vertexReflection,
        fragmentReflection);
    shaders.insert({ key, newShader });

    return newShader;
}

} // namespace URay::RHI
