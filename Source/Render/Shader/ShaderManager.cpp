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
}

Shader* ShaderManager::GetOrCreate(const std::string& key, const std::string& filePath,
                                   VkShaderStageFlagBits stage, const std::string& entry)
{
    auto it = shaders.find(key);
    if (it != shaders.end())
        return it->second;

    std::vector<char> shaderCode = ReadFile(filePath);
    if (shaderCode.empty())
        return nullptr;

    Shader* newShader = new Shader(filePath, shaderCode, stage, entry);
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
