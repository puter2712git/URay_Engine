#include "ShaderManager.h"

#include "Shader.h"

#include "Core/File/FileIO.h"

namespace URay
{

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
                                   const std::string& vertexFilePath,
                                   const std::string& fragmentFilePath)
{
    auto it = shaders.find(key);
    if (it != shaders.end())
        return it->second;

    std::vector<uint8_t> vertexShaderCode = FileIO::ReadBinary(vertexFilePath);
    if (vertexFilePath.empty())
        return nullptr;

    std::vector<uint8_t> fragmentShaderCode = FileIO::ReadBinary(fragmentFilePath);
    if (fragmentFilePath.empty())
        return nullptr;

    ShaderStage vertexStage = {};
    vertexStage.filePath = vertexFilePath;
    vertexStage.code = vertexShaderCode;
    vertexStage.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStage.entry = "VSMain";

    ShaderStage fragmentStage = {};
    fragmentStage.filePath = fragmentFilePath;
    fragmentStage.code = fragmentShaderCode;
    fragmentStage.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStage.entry = "PSMain";

    Shader* newShader = new Shader(shaderIdCounter++, vertexStage, fragmentStage);
    shaders.insert({ key, newShader });

    return newShader;
}

} // namespace URay
