#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

class DotNetRuntime;
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
    using CreateScriptFn = int32(__cdecl*)(const char*);
    using UpdateScriptFn = int32(__cdecl*)(int32, float);
    using UnloadScriptFn = int32(__cdecl*)(int32);

    CreateScriptFn createScript = nullptr;
    UpdateScriptFn updateScript = nullptr;
    UnloadScriptFn unloadScript = nullptr;

private:
    UniquePtr<DotNetRuntime> runtime = nullptr;
};

} // namespace URay
