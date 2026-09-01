#pragma once

#include "Core/File/VirtualPath.h"
#include "Core/Type/Types.h"

#include <string>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace URay
{

class VirtualFilesystem;

namespace Render
{

class Shader;

class ShaderManager
{
public:
    ShaderManager(VirtualFilesystem& filesystem);
    ~ShaderManager();

public:
    Shader* GetOrCreate(const std::string& key,
                        const VirtualPath& vertexFilePath = "",
                        const VirtualPath& fragmentFilePath = "");

private:
    VirtualFilesystem& filesystem;

    uint64 shaderIdCounter = 0;
    std::unordered_map<std::string, Shader*> shaders;
};

} // namespace Render

} // namespace URay
