#include "Window.h"

namespace URay
{

bool Window::Initialize()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindow = glfwCreateWindow(mode->width, mode->height, "URay Engine", nullptr, nullptr);

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1);

    GLFWcursor* hresizeCursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    GLFWcursor* vresizeCursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);

    cursors.insert({ CursorType::ARROW, nullptr });
    cursors.insert({ CursorType::HRESIZE, hresizeCursor });
    cursors.insert({ CursorType::VRESIZE, vresizeCursor });

    return true;
}

void Window::Finalize()
{
    for (auto& [type, cursor] : cursors)
    {
        glfwDestroyCursor(cursor);
        cursor = nullptr;
    }
    cursors.clear();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

void Window::ChangeCursor(CursorType type)
{
    auto it = cursors.find(type);
    if (it == cursors.end())
        return;

    GLFWcursor* cursor = it->second;
    glfwSetCursor(glfwWindow, cursor);
}

} // namespace URay
