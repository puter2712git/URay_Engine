#pragma once

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
    std::vector<std::unique_ptr<Window>> windows;
    Window* mainWindow = nullptr;
    bool glfwInitialized = false;
};

} // namespace URay
