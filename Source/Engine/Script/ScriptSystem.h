#pragma once

#include "Engine/Script/DotnetHost.h"

#include "Core/Type/Types.h"

#include <filesystem>

namespace URay
{

class ScriptSystem
{
public:
    ScriptSystem();
    ~ScriptSystem();

public:
    bool Initialize(const std::filesystem::path& projectPath);
    void Finalize();

    bool RunSample();

private:
    static void __cdecl LogUTF8(const uint8* message, int32 byteCount);

private:
    DotnetHost dotnetHost;
    bool isInitialized = false;
};

} // namespace URay
