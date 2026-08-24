#pragma once

#include <vulkan/vulkan.h>

namespace URay::Render
{

class PipelineLayout;

class PipelineState
{
public:
    PipelineState(VkDevice device, VkPipeline handle, PipelineLayout* layout);
    ~PipelineState();

public:
    VkPipeline GetHandle() const { return handle; }

    PipelineLayout* GetLayout() const { return layout; }

private:
    VkDevice device = VK_NULL_HANDLE;

    VkPipeline handle = VK_NULL_HANDLE;

    PipelineLayout* layout = nullptr;
};

} // namespace URay::Render
