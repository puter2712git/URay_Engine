#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

class VirtualPath;
class DotNetRuntime;

class ManagedScriptBridge
{
public:
    ManagedScriptBridge();
    ~ManagedScriptBridge();

public:
    bool Initialize(const VirtualPath& enginePath);
    void Finalize();

    int32 CreateScript(const char* typeName);
    int32 UpdateScript(int32 handle, float deltaTime);
    int32 UnloadScript(int32 handle);

private:
    UniquePtr<DotNetRuntime> runtime;

    using CreateScriptFn = int32(__cdecl*)(const char*);
    using UpdateScriptFn = int32(__cdecl*)(int32, float);
    using UnloadScriptFn = int32(__cdecl*)(int32);

    CreateScriptFn createScript = nullptr;
    UpdateScriptFn updateScript = nullptr;
    UnloadScriptFn unloadScript = nullptr;
};

} // namespace URay
