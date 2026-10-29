#include "Surface.h"

#include <Core/Log/LogSystem.h>
#include <Platform/Window/Window.h>

#include <glfw/glfw3.h>

namespace URay::Render
{

bool Surface::Initialize(VkInstance instance, Window& window)
{
    this->instance = instance;

    const VkResult result = glfwCreateWindowSurface(instance, window.GetGLFWWindow(), nullptr, &surface);
    if (result != VK_SUCCESS)
    {
        URAY_LOG("[Surface] Failed to create Vulkan surface.");
        return false;
    }

    return true;
}

void Surface::Finalize()
{
    if (surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        surface = VK_NULL_HANDLE;
    }
}

} // namespace URay::Render
