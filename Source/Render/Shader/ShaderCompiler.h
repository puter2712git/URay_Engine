#pragma once

#define NOMINMAX
#include <Windows.h>
#undef RegisterClass

#include <dxcapi.h>
#include <wrl/client.h>

#include <span>
#include <string>

namespace URay
{
class VirtualPath;
class VirtualFilesystem;
} // namespace URay

namespace URay::Render
{

class ShaderCompiler
{
public:
    ShaderCompiler(VirtualFilesystem& filesystem);
    ~ShaderCompiler();

public:
    bool Initialize();

    bool Compile(
        const VirtualPath& sourcePath,
        const VirtualPath& outputPath,
        std::wstring_view profile,
        std::wstring_view entryPoint,
        std::wstring_view includePath,
        std::span<const std::wstring> defines);

private:
    VirtualFilesystem& filesystem;

    Microsoft::WRL::ComPtr<IDxcUtils> utils = nullptr;
    Microsoft::WRL::ComPtr<IDxcCompiler3> compiler = nullptr;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
};

} // namespace URay::Render
