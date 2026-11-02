#include "ScriptSystem.h"

#include <Core/File/VirtualPath.h>
#include <Core/Log/LogSystem.h>
#include <Engine/Script/ManagedScriptBridge.h>
#include <Engine/Script/NativeAPI.h>

namespace URay
{

namespace
{
void __cdecl NativeLog(const char* message) noexcept
{
    if (!message)
        return;

    try
    {
        URAY_LOG("[C#] %s", message);
    }
    catch (...)
    {
    }
}
} // namespace

ScriptSystem::ScriptSystem() = default;

ScriptSystem::~ScriptSystem() = default;

bool ScriptSystem::Initialize(const VirtualPath& enginePath)
{
    bridge = MakeUnique<ManagedScriptBridge>();
    if (!bridge->Initialize(enginePath))
    {
        Finalize();
        return false;
    }

    return true;
}

void ScriptSystem::Finalize()
{
    if (bridge)
    {
        bridge->Finalize();
        bridge.reset();
    }
}

int32 ScriptSystem::CreateScript(const char* typeName)
{
    return bridge ? bridge->CreateScript(typeName) : 0;
}

int32 ScriptSystem::UpdateScript(int32 handle, float deltaTime)
{
    return bridge ? bridge->UpdateScript(handle, deltaTime) : -1;
}

int32 ScriptSystem::UnloadScript(int32 handle)
{
    return bridge ? bridge->UnloadScript(handle) : -1;
}

} // namespace URay
