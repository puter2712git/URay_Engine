#pragma once

#include <Core/Input/InputEvent.h>
#include <Core/Math/Extent2D.h>
#include <Core/Type/Ray/RayEmitter.h>
#include <Platform/Window/Cursor.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <unordered_map>

namespace URay
{

struct WindowDesc
{
    String title;
    int32 width;
    int32 height;
    bool resizable = true;
    bool maximized = false;
};

class Window : public RayEmitter
{
public:
    Window();
    ~Window() = default;

public:
    bool Initialize(const WindowDesc& desc);
    void Finalize();

    void ChangeCursor(CursorType type);

    Extent2D GetClientSize() const;
    Extent2D GetFramebufferSize() const;

    void RequestClose();
    bool ShouldClose() const;

    GLFWwindow* GetGLFWWindow() const { return glfwWindow; }

private:
    static void KeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow* glfwWindow, double xpos, double ypos);
    static void CharacterCallback(GLFWwindow* glfwWindow, unsigned int codepoint);
    static void ScrollCallback(GLFWwindow* glfwWindow, double xoffset, double yoffset);
    static void WindowFocusCallback(GLFWwindow* glfwWindow, int focused);
    static void FramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height);

private:
    GLFWwindow* glfwWindow = nullptr;

    std::unordered_map<CursorType, GLFWcursor*> cursors;
};

} // namespace URay
