#include "Window.h"

#include "Core/Log/LogSystem.h"

namespace URay
{

void GLFWErrorCallback(int errorCode, const char* description)
{
    URAY_LOG(
        "[GLFW] Error %d: %s",
        errorCode,
        description ? description : "No description");
}

bool Window::Initialize()
{
    glfwSetErrorCallback(GLFWErrorCallback);

    if (!glfwInit())
    {
        URAY_LOG("[Window] glfwInit failed.");
        return false;
    }
    glfwInitialized = true;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor)
    {
        URAY_LOG("[Window] Failed to get primary monitor.");
        Finalize();
        return false;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode)
    {
        URAY_LOG("[Window] Failed to get primary monitor video mode.");
        Finalize();
        return false;
    }

    glfwWindow = glfwCreateWindow(mode->width, mode->height, "URay Engine", nullptr, nullptr);
    if (!glfwWindow)
    {
        URAY_LOG("[Window] Failed to create GLFW window.");
        Finalize();
        return false;
    }

    GLFWcursor* hresizeCursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    if (!hresizeCursor)
    {
        URAY_LOG("[Window] Failed to create HRESIZE_CURSOR.");
    }

    GLFWcursor* vresizeCursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    if (!vresizeCursor)
    {
        URAY_LOG("[Window] Failed to create VRESIZE_CURSOR.");
    }

    cursors.insert({ CursorType::ARROW, nullptr });
    cursors.insert({ CursorType::HRESIZE, hresizeCursor });
    cursors.insert({ CursorType::VRESIZE, vresizeCursor });

    return true;
}

void Window::Finalize()
{
    for (auto& [type, cursor] : cursors)
    {
        if (cursor)
        {
            glfwDestroyCursor(cursor);
            cursor = nullptr;
        }
    }
    cursors.clear();

    if (glfwWindow)
    {
        glfwDestroyWindow(glfwWindow);
        glfwWindow = nullptr;
    }

    if (glfwInitialized)
    {
        glfwTerminate();
        glfwInitialized = false;
    }
}

void Window::ChangeCursor(CursorType type)
{
    auto it = cursors.find(type);
    if (it == cursors.end())
        return;

    GLFWcursor* cursor = it->second;
    glfwSetCursor(glfwWindow, cursor);
}

Extent2D Window::GetClientSize() const
{
    int32 width, height;
    glfwGetWindowSize(glfwWindow, &width, &height);

    return Extent2D{
        .width = static_cast<uint32>(width),
        .height = static_cast<uint32>(height)
    };
}

Extent2D Window::GetFramebufferSize() const
{
    int32 width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    return Extent2D{
        .width = static_cast<uint32>(width),
        .height = static_cast<uint32>(height)
    };
}

} // namespace URay
