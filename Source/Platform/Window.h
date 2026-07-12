#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace URay
{

class Window
{
public:
    Window() = default;
    ~Window() = default;

public:
    bool Initialize();
    void Finalize();

    GLFWwindow* GetGLFWWindow() const
    {
        return glfwWindow;
    }

private:
    GLFWwindow* glfwWindow = nullptr;
};

} // namespace URay
