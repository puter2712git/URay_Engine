#include "ScriptSystem.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Component/ScriptComponent.h>
#include <Engine/Engine.h>
#include <Engine/Script/NativeAPI.h>
#include <Engine/Script/ScriptMetadataRegistry.h>

#include <memory>
#include <string>
#include <vector>

namespace URay
{

ScriptSystem::ScriptSystem() = default;

ScriptSystem::~ScriptSystem() = default;

bool ScriptSystem::Initialize(
    const std::filesystem::path& enginePath,
    const std::filesystem::path& projectPath)
{
    const std::filesystem::path hostOutput =
        enginePath.parent_path() / "Source/Engine/Script/Managed/bin/Debug/net10.0";

    const std::filesystem::path gameOutput =
        projectPath / "Managed/TestManaged/bin/Debug/net10.0";

    const std::filesystem::path runtimeConfigPath = hostOutput / "URay.ScriptHost.runtimeconfig.json";

    const std::filesystem::path hostAssemblyPath = hostOutput / "URay.ScriptHost.dll";
    const std::filesystem::path gameAssemblyPath = gameOutput / "GameScripts.dll";

    metadataRegistry = std::make_unique<ScriptMetadataRegistry>(
        gEngine->GetAssetSystem().GetDatabase());

    NativeAPI nativeAPI = {
        .userData = this,
        .bootstrap = {
            .LogUTF8 = &ScriptSystem::LogUTF8,
            .RegisterScriptType = &ScriptSystem::RegisterScriptTypeUTF8,
            .RegisterScriptField = &ScriptSystem::RegisterScriptFieldUTF8 },
        .runtime = CreateRuntimeAPI()
    };

    if (!runtime.Initialize(
            runtimeConfigPath,
            hostAssemblyPath,
            nativeAPI))
        return false;

    if (!runtime.LoadGameScripts(gameAssemblyPath))
        return false;

    this->gameAssemblyPath = gameAssemblyPath;

    return true;
}

void ScriptSystem::Finalize()
{
    metadataRegistry.reset();
    instanceRegistry.Clear();
    runtime.Finalize();
    gameAssemblyPath.clear();
}

bool ScriptSystem::ReloadGameScripts()
{
    if (!runtime.IsInitialized() || gameAssemblyPath.empty())
        return false;

    const std::vector<ScriptComponent*> components =
        instanceRegistry.GetBoundComponents();

    for (ScriptComponent* component : components)
        component->ReleaseManagedInstance();

    if (!instanceRegistry.Empty())
        return false;

    if (!runtime.UnloadGameScripts())
        return false;

    metadataRegistry->Clear();

    if (!runtime.LoadGameScripts(gameAssemblyPath))
        return false;

    for (ScriptComponent* component : components)
    {
        if (!component->CreateManagedInstance())
            return false;
    }

    return true;
}

bool ScriptSystem::BindManagedInstance(uint64 instanceId, ScriptComponent& component)
{
    return instanceRegistry.Bind(instanceId, component);
}

void ScriptSystem::UnbindManagedInstance(uint64 instanceId, const ScriptComponent& component)
{
    instanceRegistry.Unbind(instanceId, component);
}

ScriptComponent* ScriptSystem::FindManagedInstance(uint64 instanceId) const
{
    return instanceRegistry.Find(instanceId);
}

void ScriptSystem::LogUTF8(void* userData, const uint8* message, int32 byteCount)
{
    if (message == nullptr || byteCount < 0)
        return;

    const char* text = reinterpret_cast<const char*>(message);
    URAY_LOG("%s", std::string(text, byteCount).c_str());
}

int32 ScriptSystem::RegisterScriptTypeUTF8(
    void* userData,
    const uint8* className,
    int32 classNameLength)
{
    if (!userData || !className || classNameLength <= 0)
        return 0;

    auto& scriptSystem = *static_cast<ScriptSystem*>(userData);

    ScriptTypeDefinition definition;
    definition.className = std::string(
        reinterpret_cast<const char*>(className),
        classNameLength);

    return scriptSystem.metadataRegistry &&
                   scriptSystem.metadataRegistry->RegisterType(std::move(definition))
               ? 1
               : 0;
}

int32 ScriptSystem::RegisterScriptFieldUTF8(
    void* userData,
    const uint8* className,
    int32 classNameLength,
    const uint8* fieldName,
    int32 fieldNameLength,
    int32 fieldType,
    const void* defaultValue,
    int32 defaultValueSize)
{
    if (!userData || !className || classNameLength <= 0 ||
        !fieldName || fieldNameLength <= 0 ||
        defaultValueSize < 0)
    {
        return 0;
    }

    auto& scriptSystem = *static_cast<ScriptSystem*>(userData);

    const std::string classNameString(reinterpret_cast<const char*>(className), classNameLength);
    const std::string fieldNameString(reinterpret_cast<const char*>(fieldName), fieldNameLength);

    return scriptSystem.metadataRegistry &&
                   scriptSystem.metadataRegistry->RegisterField(
                       classNameString,
                       fieldNameString,
                       static_cast<ScriptFieldType>(fieldType),
                       defaultValue,
                       defaultValueSize)
               ? 1
               : 0;
}

} // namespace URay
