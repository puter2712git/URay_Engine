#pragma once

#include "Platform/Window/Cursor.h"

#include "Core/Math/Extent2D.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <unordered_map>

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

    void ChangeCursor(CursorType type);

    Extent2D GetClientSize() const;
    Extent2D GetFramebufferSize() const;

    GLFWwindow* GetGLFWWindow() const { return glfwWindow; }

private:
    bool glfwInitialized = false;
    GLFWwindow* glfwWindow = nullptr;

    std::unordered_map<CursorType, GLFWcursor*> cursors;
};

} // namespace URay
