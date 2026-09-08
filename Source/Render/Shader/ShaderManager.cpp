#include "ShaderManager.h"
#include "Shader.h"
#include "ShaderReflector.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Type/Types.h"

namespace URay::Render
{

ShaderManager::ShaderManager(VirtualFilesystem& filesystem)
    : filesystem(filesystem), compiler(filesystem)
{
}

ShaderManager::~ShaderManager()
{
    Finalize();
}

bool ShaderManager::Initialize()
{
    if (!compiler.Initialize())
        return false;

    return CompileShaderPair(
               "Engine://Asset/Source/Shader/Sprite.hlsl",
               "Engine://Asset/Imported/Shader/Sprite.vert.spv",
               "Engine://Asset/Imported/Shader/Sprite.frag.spv") &&
           CompileShaderPair(
               "Engine://Asset/Source/Shader/Line.hlsl",
               "Engine://Asset/Imported/Shader/Line.vert.spv",
               "Engine://Asset/Imported/Shader/Line.frag.spv") &&
           CompileShaderPair(
               "Engine://Asset/Source/Shader/Mesh.hlsl",
               "Engine://Asset/Imported/Shader/Mesh.vert.spv",
               "Engine://Asset/Imported/Shader/Mesh.frag.spv") &&
           CompileShaderPair(
               "Engine://Asset/Source/Shader/Font.hlsl",
               "Engine://Asset/Imported/Shader/Font.vert.spv",
               "Engine://Asset/Imported/Shader/Font.frag.spv") &&
           CompileShaderPair(
               "Engine://Asset/Source/Shader/Decal.hlsl",
               "Engine://Asset/Imported/Shader/Decal.vert.spv",
               "Engine://Asset/Imported/Shader/Decal.frag.spv") &&
           CompileShaderPair(
               "Engine://Asset/Source/Shader/PostProcess/Fog.hlsl",
               "Engine://Asset/Imported/Shader/PostProcess/Fog.vert.spv",
               "Engine://Asset/Imported/Shader/PostProcess/Fog.frag.spv");
}

bool ShaderManager::CompileShaderPair(
    const VirtualPath& sourcePath,
    const VirtualPath& vertexOutputPath,
    const VirtualPath& fragmentOutputPath)
{
    const std::wstring includeDirectory =
        filesystem.ResolveToPhysicalPath("Engine://Asset/Source/Shader").wstring();
    const std::vector<std::wstring> defines;

    return compiler.Compile(
               sourcePath,
               vertexOutputPath,
               L"vs_6_0",
               L"VSMain",
               includeDirectory,
               defines) &&
           compiler.Compile(
               sourcePath,
               fragmentOutputPath,
               L"ps_6_0",
               L"PSMain",
               includeDirectory,
               defines);
}

void ShaderManager::Finalize()
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

    std::vector<uint8> vertexShaderCode = filesystem.ReadBinary(vertexFilePath);
    if (vertexShaderCode.empty())
        return nullptr;

    std::vector<uint8> fragmentShaderCode = filesystem.ReadBinary(fragmentFilePath);
    if (fragmentShaderCode.empty())
        return nullptr;

    ShaderReflection vertexReflection = {};
    if (!ShaderReflector::ReflectSPIRV(vertexShaderCode, vertexReflection))
        return nullptr;

    ShaderReflection fragmentReflection = {};
    if (!ShaderReflector::ReflectSPIRV(fragmentShaderCode, fragmentReflection))
        return nullptr;

    Shader* newShader = new Shader(
        vertexShaderCode,
        fragmentShaderCode,
        vertexReflection,
        fragmentReflection);
    shaders.insert({ key, newShader });

    return newShader;
}

} // namespace URay::Render
