#pragma once

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
    bool Initialize();
    void Run();
    void Finalize();

private:
    std::unique_ptr<Engine> engine = nullptr;
    std::unique_ptr<Editor> editor = nullptr;

    bool shouldQuit = false;
};

} // namespace URay
