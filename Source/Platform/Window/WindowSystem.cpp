#include "WindowSystem.h"

#include <Core/Log/LogSystem.h>
#include <Platform/Window/Window.h>

#include <algorithm>

namespace URay
{

void GLFWErrorCallback(int errorCode, const char* description)
{
    URAY_LOG(
        "[GLFW] Error %d: %s",
        errorCode,
        description ? description : "No description");
}

bool WindowSystem::Initialize(const WindowDesc& mainWindowDesc)
{
    glfwSetErrorCallback(GLFWErrorCallback);

    if (!glfwInit())
    {
        URAY_LOG("[WindowSystem] glfwInit failed.");
        return false;
    }
    glfwInitialized = true;

    mainWindow = CreateWindow(mainWindowDesc);
    if (!mainWindow)
    {
        URAY_LOG("[WindowSystem] Failed to create main window.");
        Finalize();
        return false;
    }

    return true;
}

void WindowSystem::Finalize()
{
    for (auto& window : windows)
    {
        if (window)
        {
            window->Finalize();
            window.reset();
        }
    }
    windows.clear();
    mainWindow = nullptr;

    if (glfwInitialized)
    {
        glfwTerminate();
        glfwInitialized = false;
    }
}

Window* WindowSystem::CreateWindow(const WindowDesc& desc)
{
    auto window = std::make_unique<Window>();
    if (!window->Initialize(desc))
    {
        URAY_LOG("[WindowSystem] Failed to initialize window.");
        return nullptr;
    }

    Window* windowPtr = window.get();
    windows.push_back(std::move(window));

    return windowPtr;
}

void WindowSystem::DestroyWindow(Window& window)
{
    if (&window == mainWindow)
    {
        URAY_LOG("[WindowSystem] Cannot destroy the main window.");
        return;
    }

    auto it = std::find_if(
        windows.begin(),
        windows.end(),
        [&window](const std::unique_ptr<Window>& wnd)
        {
            return wnd.get() == &window;
        });

    if (it == windows.end())
        return;

    (*it)->Finalize();
    windows.erase(it);
}

} // namespace URay
