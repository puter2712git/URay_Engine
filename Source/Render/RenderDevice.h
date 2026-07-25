#pragma once

#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <unordered_map>
#include <vector>

namespace URay
{

class Renderer;
class VertexBuffer;
class IndexBuffer;

struct PipelineState;

class RenderDevice
{
public:
    RenderDevice(Renderer* renderer,
                 VkPhysicalDevice physicalDevcie, VkDevice device,
                 VkQueue graphicsQueue, VkCommandPool commandPool);
    ~RenderDevice();

public:
    VertexBuffer* CreateVertexBuffer(const std::vector<Vertex>& vertices);
    IndexBuffer* CreateIndexBuffer(const std::vector<uint16_t>& indices);

    VkPipeline GetOrCreatePSO(const PipelineState& pipelineState);
    void DestroyPSOs();

private:
    VkCommandBuffer BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

    VkShaderModule CreateShaderModule(const std::vector<uint8_t>& code) const;

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

private:
    Renderer* renderer = nullptr;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    VkQueue graphicsQueue = VK_NULL_HANDLE;

    VkCommandPool commandPool = VK_NULL_HANDLE;

    std::unordered_map<uint64_t, VkPipeline> psos;
};

} // namespace URay
