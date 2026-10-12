#pragma once

#include <Core/Type/Types.h>
#include <Engine/Script/DotnetHost.h>
#include <Engine/Script/ScriptMetadata.h>

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

namespace URay
{

class ScriptAsset;

class ScriptSystem
{
public:
    bool Initialize(const std::filesystem::path& projectPath);
    void Finalize();

    bool RegisterScriptType(ScriptTypeDefinition definition);
    const ScriptTypeDefinition* FindScriptType(const std::string& className) const;

    uint64 CreateScriptInstance(std::string_view className);
    bool SetScriptFieldValue(
        uint64 instanceId,
        std::string_view fieldName,
        PropertyType type,
        const ScriptValue& value);
    bool InvokeScriptLifecycle(
        uint64 instanceId,
        ScriptLifecycle lifecycle,
        float deltaTime);
    void DestroyScriptInstance(uint64 instanceId);

private:
    static void __cdecl LogUTF8(void* userData, const uint8* message, int32 byteCount);
    static int32 __cdecl RegisterScriptTypeUTF8(void* userData, const uint8* className, int32 classNameLength);
    static int32 __cdecl RegisterScriptFieldUTF8(
        void* userData,
        const uint8* className,
        int32 classNameLength,
        const uint8* fieldName,
        int32 fieldNameLength,
        int32 fieldType,
        const void* defaultValue,
        int32 defaultValueSize);

    bool RegisterScriptField(
        const std::string& className,
        const std::string& fieldName,
        ScriptFieldType fieldType,
        const void* defaultValue,
        int32 defaultValueSize);

    ScriptAsset* FindScriptAssetByClassName(const std::string& className) const;
    bool SyncScriptAsset(const ScriptTypeDefinition& definition) const;

private:
    DotnetHost dotnetHost;
    bool isInitialized = false;

    std::unordered_map<std::string, ScriptTypeDefinition> scriptTypes;
};

} // namespace URay
