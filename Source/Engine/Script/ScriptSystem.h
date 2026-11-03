#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

class ManagedScriptBridge;
class VirtualPath;
struct ObjectId;

class ScriptSystem
{
public:
    ScriptSystem();
    ~ScriptSystem();

public:
    bool Initialize(const VirtualPath& enginePath);
    void Finalize();

    int32 CreateScript(const char* typeName, const ObjectId& ownerId);
    int32 UpdateScript(int32 handle, float deltaTime);
    int32 UnloadScript(int32 handle);

    const Vector<String>& GetScriptTypes() const;

private:
    UniquePtr<ManagedScriptBridge> bridge = nullptr;

    Vector<String> scriptTypes;
};

} // namespace URay
