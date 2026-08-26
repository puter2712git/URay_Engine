#include "Framebuffer.h"

namespace URay::Render
{

Framebuffer::Framebuffer(VkDevice device, VkFramebuffer handle)
    : device(device), handle(handle)
{
}

Framebuffer::~Framebuffer()
{
    if (handle)
    {
        vkDestroyFramebuffer(device, handle, nullptr);
    }
}

} // namespace URay::Render
