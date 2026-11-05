#include "ScriptSystem.h"

#include <Core/File/VirtualPath.h>
#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Engine.h>
#include <Engine/Object/ObjectId.h>
#include <Engine/Script/ManagedScriptBridge.h>
#include <Engine/Script/ScriptBuilder.h>

#include <algorithm>

namespace URay
{

ScriptSystem::ScriptSystem() = default;

ScriptSystem::~ScriptSystem() = default;

bool ScriptSystem::Initialize(const VirtualPath& enginePath, const VirtualPath& projectPath)
{
    Vector<String> registeredTypes;
    const bool built = ScriptBuilder(gEngine->GetAssetSystem()).Build(projectPath, registeredTypes);
    bridge = MakeUnique<ManagedScriptBridge>();
    if (!bridge->Initialize(enginePath, projectPath, built))
    {
        Finalize();
        return false;
    }

    Vector<String> discoveredTypes;
    if (!bridge->GetScriptTypes(discoveredTypes))
    {
        URAY_LOG("[ScriptSystem] Failed to discover script types.");
        Finalize();
        return false;
    }

    for (const String& typeName : registeredTypes)
    {
        if (std::find(discoveredTypes.begin(), discoveredTypes.end(), typeName) != discoveredTypes.end())
            scriptTypes.push_back(typeName);
        else if (built)
            URAY_LOG("[ScriptSystem] No matching script type for asset: %s", typeName.c_str());
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
    if (!bridge || !typeName || std::find(scriptTypes.begin(), scriptTypes.end(), typeName) == scriptTypes.end())
        return 0;
    return bridge->CreateScript(typeName, ownerId);
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
