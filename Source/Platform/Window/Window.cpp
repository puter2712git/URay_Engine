#include "Window.h"

#include <Core/Log/LogSystem.h>
#include <Platform/Input/GLFWInputAdapter.h>

namespace URay
{

void GLFWErrorCallback(int errorCode, const char* description)
{
    URAY_LOG(
        "[GLFW] Error %d: %s",
        errorCode,
        description ? description : "No description");
}

Window::Window()
{
    URAY_DECLARE_RAY(FocusChanged, bool);
    URAY_DECLARE_RAY(CursorMoved, Vector2);
    URAY_DECLARE_RAY(Scroll, Vector2);
    URAY_DECLARE_RAY(Key, KeyCode, KeyAction, ModifierKey);
    URAY_DECLARE_RAY(MouseButton, MouseButton, KeyAction, ModifierKey);
    URAY_DECLARE_RAY(TextInput, char32_t);
    URAY_DECLARE_RAY(FramebufferResized, Extent2D);
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

    glfwSetWindowUserPointer(glfwWindow, this);

    glfwSetKeyCallback(glfwWindow, KeyCallback);
    glfwSetMouseButtonCallback(glfwWindow, MouseButtonCallback);
    glfwSetCursorPosCallback(glfwWindow, CursorPosCallback);
    glfwSetCharCallback(glfwWindow, CharacterCallback);
    glfwSetScrollCallback(glfwWindow, ScrollCallback);
    glfwSetWindowFocusCallback(glfwWindow, WindowFocusCallback);
    glfwSetFramebufferSizeCallback(glfwWindow, FramebufferSizeCallback);

    GLFWcursor* textCursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    if (!textCursor)
    {
        URAY_LOG("[Window] Failed to create IBEAM_CURSOR.");
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
    cursors.insert({ CursorType::TEXT, textCursor });
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

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(glfwWindow);
}

void Window::KeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!window)
        return;

    const KeyCode keyCode = Platform::ToKeyCode(key);
    if (keyCode == KeyCode::Unknown)
        return;

    std::optional<KeyAction> keyAction = Platform::ToKeyAction(action);
    if (!keyAction.has_value())
        return;

    const ModifierKey modifierKey = Platform::ToModifierKey(mods);

    window->Emit("Key", keyCode, keyAction.value(), modifierKey);
}

void Window::MouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!window)
        return;

    MouseButton mouseButton = Platform::ToMouseButton(button);
    if (mouseButton == MouseButton::None)
        return;

    std::optional<KeyAction> mouseAction = Platform::ToKeyAction(action);
    if (!mouseAction.has_value())
        return;

    const ModifierKey modifierKey = Platform::ToModifierKey(mods);

    window->Emit("MouseButton", mouseButton, mouseAction.value(), modifierKey);
}

void Window::CursorPosCallback(GLFWwindow* glfwWindow, double xpos, double ypos)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!window)
        return;

    window->Emit("CursorMoved", Vector2(static_cast<float>(xpos), static_cast<float>(ypos)));
}

void Window::CharacterCallback(GLFWwindow* glfwWindow, unsigned int codepoint)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!window)
        return;

    if (codepoint == 0 || codepoint > 0x10FFFF)
        return;

    window->Emit("TextInput", static_cast<char32_t>(codepoint));
}

void Window::ScrollCallback(GLFWwindow* glfwWindow, double xoffset, double yoffset)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!window)
        return;

    window->Emit("Scroll", Vector2(static_cast<float>(xoffset), static_cast<float>(yoffset)));
}

void Window::WindowFocusCallback(GLFWwindow* glfwWindow, int focused)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!window)
        return;

    window->Emit("FocusChanged", focused == GLFW_TRUE);
}

void Window::FramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!window)
        return;

    window->Emit("FramebufferResized", Extent2D{ .width = static_cast<uint32>(width), .height = static_cast<uint32>(height) });
}

} // namespace URay
