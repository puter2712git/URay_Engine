#pragma once

#include <Core/Type/Types.h>
#include <Engine/Script/DotnetHost.h>
#include <Engine/Script/ScriptMetadata.h>

#include <filesystem>
#include <string>
#include <unordered_map>

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

    bool RegisterScriptType(ScriptTypeDefinition definition);
    const ScriptTypeDefinition* FindScriptType(const std::string& className) const;

private:
    static void __cdecl LogUTF8(const uint8* message, int32 byteCount);

private:
    DotnetHost dotnetHost;
    bool isInitialized = false;

    std::unordered_map<std::string, ScriptTypeDefinition> scriptTypes;
};

} // namespace URay
