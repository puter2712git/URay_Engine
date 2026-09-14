#pragma once

#include <vulkan/vulkan.h>

#include <memory>

namespace URay::Render
{

class Buffer;
class CommandBuffer;
class DescriptorSet;

struct FrameResource
{
    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence inFlightFence = VK_NULL_HANDLE;

    std::unique_ptr<CommandBuffer> commandBuffer = nullptr;
    std::unique_ptr<Buffer> uniformBuffer = nullptr;
    std::unique_ptr<Buffer> pointLightStorageBuffer = nullptr;
    std::unique_ptr<Buffer> spotLightStorageBuffer = nullptr;
    std::unique_ptr<DescriptorSet> descriptorSet = nullptr;
};

} // namespace URay::Render
