#pragma once

#include <Core/Type/Types.h>
#include <Engine/Script/ScriptInstanceRegistry.h>
#include <Engine/Script/ScriptRuntime.h>

#include <filesystem>
#include <memory>

namespace URay
{

class ScriptComponent;
class ScriptMetadataRegistry;

class ScriptSystem
{
public:
    ScriptSystem();
    ~ScriptSystem();

    bool Initialize(const std::filesystem::path& enginePath, const std::filesystem::path& projectPath);
    void Finalize();

    bool ReloadGameScripts();

    ScriptRuntime& GetRuntime() { return runtime; }
    const ScriptRuntime& GetRuntime() const { return runtime; }

    bool BindManagedInstance(uint64 instanceId, ScriptComponent& component);
    void UnbindManagedInstance(uint64 instanceId, const ScriptComponent& component);
    ScriptComponent* FindManagedInstance(uint64 instanceId) const;

private:
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
    ScriptRuntime runtime;

    std::filesystem::path gameAssemblyPath;

    std::unique_ptr<ScriptMetadataRegistry> metadataRegistry;
    ScriptInstanceRegistry instanceRegistry;
};

} // namespace URay
