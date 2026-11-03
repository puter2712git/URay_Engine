#include "ScriptSystem.h"

#include <Core/File/VirtualPath.h>
#include <Core/Log/LogSystem.h>
#include <Engine/Object/ObjectId.h>
#include <Engine/Script/ManagedScriptBridge.h>

namespace URay
{

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

    if (!bridge->GetScriptTypes(scriptTypes))
    {
        URAY_LOG("[ScriptSystem] Failed to discover script types.");
        Finalize();
        return false;
    }

    for (const String& typeName : scriptTypes)
    {
        URAY_LOG("[ScriptSystem] Discovered script: %s", typeName.c_str());
    }

    return true;
}

void ScriptSystem::Finalize()
{
    scriptTypes.clear();

    if (bridge)
    {
        bridge->Finalize();
        bridge.reset();
    }
}

int32 ScriptSystem::CreateScript(const char* typeName, const ObjectId& ownerId)
{
    return bridge ? bridge->CreateScript(typeName, ownerId) : 0;
}

int32 ScriptSystem::UpdateScript(int32 handle, float deltaTime)
{
    return bridge ? bridge->UpdateScript(handle, deltaTime) : -1;
}

int32 ScriptSystem::UnloadScript(int32 handle)
{
    return bridge ? bridge->UnloadScript(handle) : -1;
}

const Vector<String>& ScriptSystem::GetScriptTypes() const
{
    return scriptTypes;
}

} // namespace URay
