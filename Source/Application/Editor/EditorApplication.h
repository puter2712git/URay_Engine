#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

class Engine;
class Editor;

class EditorApplication
{
public:
    EditorApplication();
    ~EditorApplication();

public:
    bool Initialize(const String& enginePath, const String& projectPath);
    void Run();
    void Finalize();

private:
    UniquePtr<Engine> engine = nullptr;
    UniquePtr<Editor> editor = nullptr;
};

} // namespace URay
