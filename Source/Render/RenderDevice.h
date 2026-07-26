#pragma once

#include "Render/Vertex.h"

#include <vulkan/vulkan.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace URay
{

class Renderer;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class Texture;
class TextureView;

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

    Texture* GetOrCreateTexture(const std::string& filePath);
    void DestroyTextures();

    TextureView* GetOrCreateTextureView(Texture* texture);
    void DestroyTextureViews();

    VkPipeline GetOrCreatePSO(const PipelineState& pipelineState);
    void DestroyPSOs();

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

    bool CreateImage(uint32_t width, uint32_t height,
                     VkFormat format, VkImageTiling tiling,
                     VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                     VkImage& image, VkDeviceMemory& imageMemory) const;
    void TransitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout) const;
    void CopyBufferToImage(VkBuffer buffer, VkImage image,
                           uint32_t width, uint32_t height) const;
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const;

    const std::vector<ConstantBuffer*>& GetFrameConstantBuffers() const { return frameConstantBuffers; }

private:
    VkCommandBuffer BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

    void CreatePersistentVertexBuffer();
    void DestroyPersistentVertexBuffer();

    VkShaderModule CreateShaderModule(const std::vector<uint8_t>& code) const;

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

private:
    Renderer* renderer = nullptr;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    VkQueue graphicsQueue = VK_NULL_HANDLE;

    VkCommandPool commandPool = VK_NULL_HANDLE;

    std::unordered_map<std::string, Texture*> textures;
    std::unordered_map<Texture*, TextureView*> textureViews;

    std::unordered_map<uint64_t, VkPipeline> pipelines;

    std::vector<ConstantBuffer*> frameConstantBuffers;

public:
    VkBuffer persistentVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory persistentVertexBufferMemory = VK_NULL_HANDLE;
    void* mappedPersistentVertexBufferData = nullptr;
    uint32_t vertexCount = 0;
};

} // namespace URay
