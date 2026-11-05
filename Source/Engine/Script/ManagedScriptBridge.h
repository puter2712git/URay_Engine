#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

class VirtualPath;
class DotNetRuntime;
struct ObjectId;

class ManagedScriptBridge
{
public:
    ManagedScriptBridge();
    ~ManagedScriptBridge();

public:
    bool Initialize(const VirtualPath& enginePath, const VirtualPath& projectPath, bool loadProjectAssembly);
    void Finalize();

    int32 CreateScript(const char* typeName, const ObjectId& ownerId);
    int32 UpdateScript(int32 handle, float deltaTime);
    int32 UnloadScript(int32 handle);

    bool GetScriptTypes(Vector<String>& outTypes);

private:
    using CreateScriptFn = int32(__cdecl*)(const char*, int32, uint32);
    using UpdateScriptFn = int32(__cdecl*)(int32, float);
    using UnloadScriptFn = int32(__cdecl*)(int32);

    using CollectScriptTypeFn = int32(__cdecl*)(void*, const char*);
    using EnumerateScriptTypesFn = int32(__cdecl*)(CollectScriptTypeFn, void*);

private:
    UniquePtr<DotNetRuntime> runtime;

    CreateScriptFn createScript = nullptr;
    UpdateScriptFn updateScript = nullptr;
    UnloadScriptFn unloadScript = nullptr;

    EnumerateScriptTypesFn enumerateScriptTypes = nullptr;
};

} // namespace URay
