#pragma once

#include <vulkan/vulkan.h>

namespace URay
{
class Window;
}

namespace URay::Render
{

class Surface
{
public:
    bool Initialize(VkInstance instance, Window& window);
    void Finalize();

    VkSurfaceKHR GetSurface() const { return surface; }

private:
    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
};

} // namespace URay::Render
