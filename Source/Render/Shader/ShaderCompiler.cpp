#include "ShaderCompiler.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/File/VirtualPath.h"
#include "Core/Log/LogSystem.h"

#include <vector>

namespace URay::Render
{

using Microsoft::WRL::ComPtr;

ShaderCompiler::ShaderCompiler() = default;

ShaderCompiler::~ShaderCompiler() = default;

bool ShaderCompiler::Initialize()
{
    HRESULT result = DxcCreateInstance(
        CLSID_DxcUtils,
        IID_PPV_ARGS(&utils));

    if (FAILED(result))
    {
        URAY_LOG(
            "[ShaderCompiler] Failed to create DXC utils. (HRESULT: 0x%08X)",
            static_cast<unsigned int>(result));
        return false;
    }

    result = DxcCreateInstance(
        CLSID_DxcCompiler,
        IID_PPV_ARGS(&compiler));

    if (FAILED(result))
    {
        URAY_LOG(
            "[ShaderCompiler] Failed to create DXC compiler. (HRESULT: 0x%08X)",
            static_cast<unsigned int>(result));
        return false;
    };
    result = utils->CreateDefaultIncludeHandler(&includeHandler);
    if (FAILED(result))
    {
        URAY_LOG(
            "[ShaderCompiler] Failed to create DXC include handler. (HRESULT: 0x%08X)",
            static_cast<unsigned int>(result));
        return false;
    }

    return true;
}

bool ShaderCompiler::Compile(
    const VirtualPath& sourcePath,
    const VirtualPath& outputPath,
    std::wstring_view profile,
    std::wstring_view entryPoint,
    std::wstring_view includePath,
    std::span<const std::wstring> defines)
{
    if (!utils || !compiler || !includeHandler)
    {
        URAY_LOG("[ShaderCompiler] Compiler is not initialized.");
        return false;
    }

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    const std::wstring sourcePhysicalPath = fileSystem.ResolveToPhysicalPath(sourcePath).wstring();

    if (sourcePhysicalPath.empty())
    {
        URAY_LOG("[ShaderCompiler] Failed to resolve shader path: %s", sourcePath.ToString().c_str());
        return false;
    }

    ComPtr<IDxcBlobEncoding> source;
    const HRESULT loadResult = utils->LoadFile(sourcePhysicalPath.c_str(), nullptr, &source);

    if (FAILED(loadResult))
    {
        URAY_LOG(
            "[ShaderCompiler] Failed to load shader source: %s (HRESULT: 0x%08X)",
            sourcePath.ToString().c_str(),
            static_cast<unsigned int>(loadResult));
        return false;
    }

    DxcBuffer sourceBuffer = {
        .Ptr = source->GetBufferPointer(),
        .Size = source->GetBufferSize(),
        .Encoding = DXC_CP_UTF8
    };

    std::vector<LPCWSTR> args = {
        L"-spirv",
        L"-T", profile.data(),
        L"-E", entryPoint.data(),
        L"-I", includePath.data()
    };

    for (const std::wstring& define : defines)
    {
        args.push_back(L"-D");
        args.push_back(define.c_str());
    }

    ComPtr<IDxcResult> result;
    const HRESULT compileResult = compiler->Compile(
        &sourceBuffer,
        args.data(),
        static_cast<uint32_t>(args.size()),
        includeHandler.Get(),
        IID_PPV_ARGS(&result));

    if (FAILED(compileResult))
    {
        URAY_LOG(
            "[ShaderCompiler] DXC compile invocation failed: %s (HRESULT: 0x%08X)",
            sourcePath.ToString().c_str(),
            static_cast<unsigned int>(compileResult));
        return false;
    }

    HRESULT compileStatus = E_FAIL;
    const HRESULT statusResult = result->GetStatus(&compileStatus);

    if (FAILED(statusResult))
    {
        URAY_LOG(
            "[ShaderCompiler] Failed to get compile status: %s (HRESULT: 0x%08X)",
            sourcePath.ToString().c_str(),
            static_cast<unsigned int>(statusResult));
        return false;
    }

    ComPtr<IDxcBlobUtf8> diagnostics;
    const HRESULT diagnosticsResult = result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&diagnostics), nullptr);

    if (FAILED(diagnosticsResult))
    {
        URAY_LOG(
            "[ShaderCompiler] Failed to get DXC diagnostics: %s (HRESULT: 0x%08X)",
            sourcePath.ToString().c_str(),
            static_cast<unsigned int>(diagnosticsResult));
    }

    if (diagnostics && diagnostics->GetStringLength() > 0)
    {
        const std::string diagnosticsText(diagnostics->GetStringPointer(), diagnostics->GetStringLength());

        URAY_LOG(
            "[ShaderCompiler] DXC diagnostics for %s (%ls, %ls):\n%s",
            sourcePath.ToString().c_str(),
            std::wstring(profile).c_str(),
            std::wstring(entryPoint).c_str(),
            diagnosticsText.c_str());
    }

    if (FAILED(compileStatus))
    {
        URAY_LOG(
            "[ShaderCompiler] Compilation failed: %s (%ls, %ls)",
            sourcePath.ToString().c_str(),
            std::wstring(profile).c_str(),
            std::wstring(entryPoint).c_str());

        return false;
    }

    ComPtr<IDxcBlob> spirv;
    const HRESULT objectResult = result->GetOutput(
        DXC_OUT_OBJECT,
        IID_PPV_ARGS(&spirv),
        nullptr);

    if (FAILED(objectResult))
    {
        URAY_LOG(
            "[ShaderCompiler] Failed to get SPIR-V output: %s (HRESULT: 0x%08X)",
            sourcePath.ToString().c_str(),
            static_cast<unsigned int>(objectResult));
        return false;
    }

    std::vector<uint8> bytes(
        static_cast<const uint8*>(spirv->GetBufferPointer()),
        static_cast<const uint8*>(spirv->GetBufferPointer()) +
            spirv->GetBufferSize());

    if (!fileSystem.WriteBinary(outputPath, bytes))
    {
        URAY_LOG(
            "[ShaderCompiler] Failed to write SPIR-V output: %s",
            outputPath.ToString().c_str());
        return false;
    }

    return true;
}

} // namespace URay::Render
