#pragma once

#include <Core/Type/TypeAlias.h>
#include <Engine/Script/ScriptField.h>

namespace URay
{

class VirtualPath;
class DotNetRuntime;
struct ObjectId;

struct ScriptFieldTransfer
{
    const char* declaringTypeName;
    const char* name;
    int32 type;
    const uint8* data;
    int32 size;
};

class ManagedScriptBridge
{
public:
    ManagedScriptBridge();
    ~ManagedScriptBridge();

public:
    bool Initialize(const VirtualPath& enginePath, const VirtualPath& projectPath, bool loadProjectAssembly);
    void Finalize();

    int32 CreateScript(const char* typeName, const ObjectId& ownerId, const Vector<ScriptFieldOverride>& overrides);
    int32 UpdateScript(int32 handle, float deltaTime);
    int32 UnloadScript(int32 handle);

    bool GetScriptTypes(Vector<String>& outTypes);
    bool GetScriptFields(const char* typeName, Vector<ScriptFieldInfo>& outFields);

private:
    using CreateScriptFn = int32(__cdecl*)(const char*, int32, uint32, const ScriptFieldTransfer*, int32);
    using UpdateScriptFn = int32(__cdecl*)(int32, float);
    using UnloadScriptFn = int32(__cdecl*)(int32);

    using CollectScriptTypeFn = int32(__cdecl*)(void*, const char*);
    using EnumerateScriptTypesFn = int32(__cdecl*)(CollectScriptTypeFn, void*);
    using CollectScriptFieldFn = int32(__cdecl*)(void*, const char*, const char*, int32, const uint8*, int32);
    using EnumerateScriptFieldsFn = int32(__cdecl*)(const char*, CollectScriptFieldFn, void*);

private:
    UniquePtr<DotNetRuntime> runtime;

    CreateScriptFn createScript = nullptr;
    UpdateScriptFn updateScript = nullptr;
    UnloadScriptFn unloadScript = nullptr;

    EnumerateScriptTypesFn enumerateScriptTypes = nullptr;
    EnumerateScriptFieldsFn enumerateScriptFields = nullptr;
};

} // namespace URay
