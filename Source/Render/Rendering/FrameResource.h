#pragma once

#include <Core/Type/TypeAlias.h>
#include <Render/Rendering/RenderPass/RenderPassId.h>

#include <vulkan/vulkan.h>

#include <array>
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
    std::array<bool, static_cast<uint32>(RenderPassId::Count)> recordedPasses = {};
};

struct FrameResource
{
    VkFence inFlightFence = VK_NULL_HANDLE;

    GPUTimeStampState gpuTimeStamps = {};

    UniquePtr<CommandBuffer> commandBuffer = nullptr;
    UniquePtr<Buffer> uniformBuffer = nullptr;
    UniquePtr<Buffer> pointLightStorageBuffer = nullptr;
    UniquePtr<Buffer> spotLightStorageBuffer = nullptr;
    UniquePtr<Buffer> shadowUniformBuffer = nullptr;
    UniquePtr<Buffer> spotLightShadowStorageBuffer = nullptr;
    UniquePtr<Buffer> pointLightShadowStorageBuffer = nullptr;
    UniquePtr<Buffer> clusterLightListStorageBuffer = nullptr;
    UniquePtr<Buffer> clusterLightIndexListStorageBuffer = nullptr;

    UniquePtr<DescriptorSet> descriptorSet = nullptr;
};

} // namespace URay::Render
