#pragma once

#include <vulkan/vulkan.h>

namespace URay::RHI
{

class DescriptorSetLayout;
class ConstantBuffer;
class TextureView;

class DescriptorSet
{
public:
    DescriptorSet(VkDevice device, VkDescriptorSet handle, DescriptorSetLayout* layout);
    ~DescriptorSet();

public:
    void WriteUniformBuffer(
        uint32_t binding,
        const ConstantBuffer* buffer,
        VkDeviceSize offset = 0,
        VkDeviceSize range = VK_WHOLE_SIZE);

    void WriteSampledImage(
        uint32_t binding,
        const TextureView* textureView,
        VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    void WriteSampler(
        uint32_t binding,
        VkSampler sampler);

    VkDescriptorSet GetHandle() const
    {
        return handle;
    }

    DescriptorSetLayout* GetLayout() const { return layout; }

private:
    VkDevice device = VK_NULL_HANDLE;
    VkDescriptorSet handle = VK_NULL_HANDLE;

    DescriptorSetLayout* layout = nullptr;
};

} // namespace URay::RHI
