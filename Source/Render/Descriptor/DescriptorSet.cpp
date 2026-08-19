#include "DescriptorSet.h"

#include "Render/Buffer/ConstantBuffer.h"
#include "Render/Texture/TextureView.h"

namespace URay::RHI
{

DescriptorSet::DescriptorSet(VkDevice device, VkDescriptorSet handle, DescriptorSetLayout* layout)
    : device(device), handle(handle), layout(layout)
{
}

DescriptorSet::~DescriptorSet()
{
}

void DescriptorSet::WriteUniformBuffer(
    uint32_t binding,
    const ConstantBuffer* buffer,
    VkDeviceSize offset,
    VkDeviceSize range)
{
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = buffer->GetHandle();
    bufferInfo.offset = offset;
    bufferInfo.range = range;

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = handle;
    write.dstBinding = binding;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void DescriptorSet::WriteSampledImage(
    uint32_t binding,
    const TextureView* textureView,
    VkImageLayout layout)
{
    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = layout;
    imageInfo.imageView = textureView->GetHandle();

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = handle;
    write.dstBinding = binding;
    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void DescriptorSet::WriteSampler(
    uint32_t binding,
    VkSampler sampler)
{
    VkDescriptorImageInfo samplerInfo = {};
    samplerInfo.sampler = sampler;

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = handle;
    write.dstBinding = binding;
    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &samplerInfo;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

} // namespace URay::RHI
