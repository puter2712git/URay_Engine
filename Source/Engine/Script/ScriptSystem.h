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

private:
    UniquePtr<DotNetRuntime> runtime = nullptr;
};

} // namespace URay
