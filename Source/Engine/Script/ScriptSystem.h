#pragma once

#include <Core/Type/Types.h>
#include <Engine/Script/DotnetHost.h>
#include <Engine/Script/ScriptMetadata.h>

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace URay
{

class ScriptComponent;
class ScriptMetadataRegistry;
class TransformComponent;

class ScriptSystem
{
public:
    ScriptSystem() = default;
    ~ScriptSystem();

    bool Initialize(const std::filesystem::path& enginePath, const std::filesystem::path& projectPath);
    void Finalize();

    bool ReloadGameScripts();

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

    bool BindManagedInstance(uint64 instanceId, ScriptComponent& component);
    void UnbindManagedInstance(uint64 instanceId, const ScriptComponent& component);
    ScriptComponent* FindManagedInstance(uint64 instanceId) const;

    TransformComponent* FindTransformForManagedInstance(uint64 instanceId) const;

private:
    TransformComponent* ResolveTransform(uint64 instanceId) const;

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

private:
    DotnetHost dotnetHost;
    bool isInitialized = false;

    std::filesystem::path gameAssemblyPath;

    std::unique_ptr<ScriptMetadataRegistry> metadataRegistry;
    std::unordered_map<uint64, ScriptComponent*> managedInstances;
};

} // namespace URay
