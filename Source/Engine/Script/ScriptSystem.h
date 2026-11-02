#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

class ManagedScriptBridge;
class VirtualPath;

class ScriptSystem
{
public:
    ScriptSystem();
    ~ScriptSystem();

public:
    bool Initialize(const VirtualPath& enginePath);
    void Finalize();

    int32 CreateScript(const char* typeName);
    int32 UpdateScript(int32 handle, float deltaTime);
    int32 UnloadScript(int32 handle);

private:
    UniquePtr<ManagedScriptBridge> bridge = nullptr;
};

} // namespace URay
