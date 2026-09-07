#include "ShaderCompiler.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/File/VirtualPath.h"

#include <iostream>
#include <vector>

namespace URay::Render
{

using Microsoft::WRL::ComPtr;

ShaderCompiler::ShaderCompiler(VirtualFilesystem& filesystem)
    : filesystem(filesystem) {}

ShaderCompiler::~ShaderCompiler() = default;

bool ShaderCompiler::Initialize()
{
    if (FAILED(DxcCreateInstance(
            CLSID_DxcUtils,
            IID_PPV_ARGS(&utils))))
    {
        return false;
    }

    if (FAILED(DxcCreateInstance(
            CLSID_DxcCompiler,
            IID_PPV_ARGS(&compiler))))
    {
        return false;
    }

    if (FAILED(utils->CreateDefaultIncludeHandler(&includeHandler)))
    {
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
        return false;

    const std::wstring sourcePhysicalPath =
        filesystem.ResolveToPhysicalPath(sourcePath).wstring();

    ComPtr<IDxcBlobEncoding> source;
    if (FAILED(utils->LoadFile(sourcePhysicalPath.c_str(), nullptr, &source)))
        return false;

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
    if (FAILED(compiler->Compile(
            &sourceBuffer,
            args.data(),
            static_cast<uint32_t>(args.size()),
            includeHandler.Get(),
            IID_PPV_ARGS(&result))))
    {
        return false;
    }

    HRESULT compileStatus = E_FAIL;
    result->GetStatus(&compileStatus);

    ComPtr<IDxcBlobUtf8> diagnostics;
    result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&diagnostics), nullptr);

    if (diagnostics && diagnostics->GetStringLength() > 0)
        std::cerr << diagnostics->GetStringPointer();

    if (FAILED(compileStatus))
        return false;

    ComPtr<IDxcBlob> spirv;
    if (FAILED(result->GetOutput(
            DXC_OUT_OBJECT,
            IID_PPV_ARGS(&spirv),
            nullptr)))
    {
        return false;
    }

    const auto* data = static_cast<const uint8*>(spirv->GetBufferPointer());
    const size_t size = spirv->GetBufferSize();

    std::vector<uint8> bytes(data, data + size);

    return filesystem.WriteBinary(outputPath, bytes);
}

} // namespace URay::Render
