#pragma once

#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace URay
{

class VertexBuffer;
class IndexBuffer;

class RenderDevice
{
public:
    RenderDevice(VkPhysicalDevice physicalDevcie, VkDevice device,
                 VkQueue graphicsQueue, VkCommandPool commandPool);

public:
    VertexBuffer* CreateVertexBuffer(const std::vector<Vertex>& vertices);
    IndexBuffer* CreateIndexBuffer(const std::vector<uint16_t>& indices);

private:
    VkCommandBuffer BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    VkQueue graphicsQueue = VK_NULL_HANDLE;

    VkCommandPool commandPool = VK_NULL_HANDLE;
};

} // namespace URay
