#pragma once

#include <vulkan/vulkan.h>

namespace URay
{

class PipelineLayout;

class PipelineStateObject
{
public:
    PipelineStateObject(VkDevice device, VkPipeline handle, PipelineLayout* layout);
    ~PipelineStateObject();

public:
    VkPipeline GetHandle() const { return handle; }

    PipelineLayout* GetLayout() const { return layout; }

private:
    VkDevice device = VK_NULL_HANDLE;

    VkPipeline handle = VK_NULL_HANDLE;

    PipelineLayout* layout = nullptr;
};

} // namespace URay
