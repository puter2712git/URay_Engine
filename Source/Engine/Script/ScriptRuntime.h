#pragma once

#include <Core/Type/Types.h>
#include <Engine/Script/DotnetHost.h>
#include <Engine/Script/ScriptMetadata.h>

#include <filesystem>
#include <string_view>

namespace URay
{

struct NativeAPI;

class ScriptRuntime
{
public:
    bool Initialize(
        const std::filesystem::path& runtimeConfigPath,
        const std::filesystem::path& hostAssemblyPath,
        const NativeAPI& nativeAPI);

    void Finalize();

    bool LoadGameScripts(const std::filesystem::path& assemblyPath);
    bool UnloadGameScripts();

    uint64 CreateInstance(std::string_view className);

    bool SetFieldValue(
        uint64 instanceId,
        std::string_view fieldName,
        PropertyType type,
        const ScriptValue& value);

    bool InvokeLifecycle(
        uint64 instanceId,
        ScriptLifecycle lifecycle,
        float deltaTime);

    void DestroyInstance(uint64 instanceId);

    bool IsInitialized() const { return isInitialized; }

private:
    DotnetHost dotnetHost;
    bool isInitialized = false;
};

} // namespace URay
