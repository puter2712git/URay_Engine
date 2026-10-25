#pragma once
#include <Core/Type/Types.h>

#include <memory>

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
    std::unique_ptr<Engine> engine = nullptr;
    std::unique_ptr<Editor> editor = nullptr;
};

} // namespace URay
