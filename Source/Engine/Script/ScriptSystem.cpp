#include "ScriptSystem.h"

#include "Engine/Script/NativeAPI.h"

#include "Core/Log/Log.h"

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
    isInitialized = false;
}

bool ScriptSystem::RunSample()
{
    if (!isInitialized)
        return false;

    return dotnetHost.RunSample();
}

void ScriptSystem::LogUTF8(const uint8* message, int32 byteCount)
{
    if (message == nullptr || byteCount < 0)
        return;

    const char* utf8 = reinterpret_cast<const char*>(message);

    Logger::Log(std::string(utf8, static_cast<size_t>(byteCount)));
}

} // namespace URay
