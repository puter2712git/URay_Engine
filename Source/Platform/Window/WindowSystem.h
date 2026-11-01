#pragma once

#include <Core/Type/TypeAlias.h>
#include <memory>
#include <vector>

namespace URay
{

class Window;
struct WindowDesc;

class WindowSystem
{
public:
    bool Initialize(const WindowDesc& mainWindowDesc);
    void Finalize();

    Window* CreateWindow(const WindowDesc& desc);
    void DestroyWindow(Window& window);

    Window& GetMainWindow() const { return *mainWindow; }

private:
    Vector<UniquePtr<Window>> windows;
    Window* mainWindow = nullptr;
    bool glfwInitialized = false;
};

} // namespace URay
