#pragma once

#include <vulkan/vulkan.h>

#include <span>

namespace URay::Render
{

struct FramebufferDesc
{
    VkRenderPass renderPass = VK_NULL_HANDLE;
    std::span<const VkImageView> attachments;
    VkExtent2D extent = {};
};

class Framebuffer
{
public:
    Framebuffer(VkDevice device, VkFramebuffer handle);
    ~Framebuffer();

public:
    VkFramebuffer GetHandle() const { return handle; }

private:
    VkDevice device = VK_NULL_HANDLE;
    VkFramebuffer handle = VK_NULL_HANDLE;
};

} // namespace URay::Render
