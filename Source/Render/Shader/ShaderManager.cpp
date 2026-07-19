#include "ShaderManager.h"

#include "Shader.h"

#include <fstream>
#include <vector>

namespace
{

std::vector<char> ReadFile(const std::string& fileName);

}

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

    std::vector<char> vertexShaderCode = ReadFile(vertexFilePath);
    if (vertexFilePath.empty())
        return nullptr;

    std::vector<char> fragmentShaderCode = ReadFile(fragmentFilePath);
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

    Shader* newShader = new Shader(vertexStage, fragmentStage);
    shaders.insert({ key, newShader });

    newShader->SetId(shaderIdCounter++);

    return newShader;
}

} // namespace URay

namespace
{

std::vector<char> ReadFile(const std::string& fileName)
{
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        return std::vector<char>();

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

} // namespace
