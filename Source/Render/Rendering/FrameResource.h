#pragma once

#include <Core/Type/Types.h>

#include <vulkan/vulkan.h>

#include <memory>

namespace URay::Render
{

class Buffer;
class CommandBuffer;
class DescriptorSet;

struct GPUTimeStampState
{
    VkQueryPool queryPool = VK_NULL_HANDLE;

    bool hasPendingResults = false;
    uint64 submissionFrameNumber = 0;
};

struct FrameResource
{
    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkFence inFlightFence = VK_NULL_HANDLE;

    GPUTimeStampState gpuTimeStamps = {};

    std::unique_ptr<CommandBuffer> commandBuffer = nullptr;
    std::unique_ptr<Buffer> uniformBuffer = nullptr;
    std::unique_ptr<Buffer> pointLightStorageBuffer = nullptr;
    std::unique_ptr<Buffer> spotLightStorageBuffer = nullptr;
    std::unique_ptr<Buffer> shadowUniformBuffer = nullptr;
    std::unique_ptr<Buffer> spotLightShadowStorageBuffer = nullptr;
    std::unique_ptr<Buffer> pointLightShadowStorageBuffer = nullptr;
    std::unique_ptr<Buffer> clusterLightListStorageBuffer = nullptr;
    std::unique_ptr<Buffer> clusterLightIndexListStorageBuffer = nullptr;

    std::unique_ptr<DescriptorSet> descriptorSet = nullptr;
};

} // namespace URay::Render
