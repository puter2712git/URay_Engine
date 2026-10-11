#include "ScriptSystem.h"

#include <Engine/Script/NativeAPI.h>

#include <Core/Log/LogSystem.h>

namespace URay
{

ScriptSystem::ScriptSystem() = default;

ScriptSystem::~ScriptSystem() = default;

bool ScriptSystem::Initialize(const std::filesystem::path& projectPath)
{
    const std::filesystem::path managedOutput =
        projectPath / "Managed" / "TestManaged" / "bin" / "Debug" / "net10.0";

    const std::filesystem::path runtimeConfigPath = managedOutput / "URay.runtimeconfig.json";

    const std::filesystem::path assemblyPath = managedOutput / "URay.dll";

    if (!dotnetHost.Initialize(runtimeConfigPath, assemblyPath))
        return false;

    NativeAPI nativeAPI = {
        .LogUTF8 = &ScriptSystem::LogUTF8
    };

    if (!dotnetHost.InitializeManagedAPI(nativeAPI))
        return false;

    isInitialized = true;
    return true;
}

void ScriptSystem::Finalize()
{
    scriptTypes.clear();
    isInitialized = false;
}

bool ScriptSystem::RunSample()
{
    if (!isInitialized)
        return false;

    return dotnetHost.RunSample();
}

bool ScriptSystem::RegisterScriptType(ScriptTypeDefinition definition)
{
    if (definition.className.empty())
        return false;

    scriptTypes.insert_or_assign(definition.className, std::move(definition));
    return true;
}

const ScriptTypeDefinition* ScriptSystem::FindScriptType(const std::string& className) const
{
    const auto it = scriptTypes.find(className);
    return it != scriptTypes.end() ? &it->second : nullptr;
}

void ScriptSystem::LogUTF8(const uint8* message, int32 byteCount)
{
    if (message == nullptr || byteCount < 0)
        return;

    const char* text = reinterpret_cast<const char*>(message);
    URAY_LOG("%s", std::string(text, byteCount).c_str());
}

} // namespace URay
