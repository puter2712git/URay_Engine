#include "DescriptorSet.h"

#include "Render/RHI/Buffer/ConstantBuffer.h"
#include "Render/RHI/Texture/TextureView.h"

#include "Core/Type/Types.h"

namespace URay::Render
{

DescriptorSet::DescriptorSet(VkDevice device, VkDescriptorSet handle, DescriptorSetLayout* layout)
    : device(device), handle(handle), layout(layout)
{
}

DescriptorSet::~DescriptorSet()
{
}

void DescriptorSet::WriteUniformBuffer(
    uint32 binding,
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
    uint32 binding,
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
    uint32 binding,
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

} // namespace URay::Render
