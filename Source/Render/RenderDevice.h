#pragma once

#include "Render/Vertex.h"

#include <spirv/spirv_reflect.h>
#include <vulkan/vulkan.h>

#include <span>
#include <string>
#include <vector>

namespace URay
{
class Texture;
}

namespace URay::Render
{

class Renderer;
class VertexBuffer;
class IndexBuffer;
class MeshBuffer;
class ConstantBuffer;
class Texture;
class TextureView;
class DescriptorSetLayout;
class DescriptorSet;
class PipelineLayout;
class PipelineState;

struct TextureDesc;
struct TextureSamplerDesc;
struct DescriptorSetLayoutDesc;
struct PipelineLayoutDesc;
struct PipelineStateDesc;

class RenderDevice
{
public:
    RenderDevice(Renderer* renderer,
                 VkPhysicalDevice physicalDevcie, VkDevice device,
                 VkQueue graphicsQueue, VkCommandPool commandPool);
    ~RenderDevice();

public:
    VertexBuffer* CreateVertexBuffer(const std::vector<VertexPNT>& vertices);
    IndexBuffer* CreateIndexBuffer(const std::vector<uint32_t>& indices);
    MeshBuffer* CreateMeshBuffer(VertexBuffer* inVertexBuffer, IndexBuffer* inIndexBuffer);

    Texture* CreateTexture(const TextureDesc& desc);
    bool UploadTextureData(Texture* texture, std::span<const uint8_t> pixelData);

    TextureView* CreateTextureView(Texture* texture);
    VkSampler CreateTextureSampler(const TextureSamplerDesc& samplerDesc);

    DescriptorSetLayout* CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc);
    DescriptorSet* CreateDescriptorSet(DescriptorSetLayout* layout);

    PipelineLayout* CreatePipelineLayout(const PipelineLayoutDesc& desc);

    PipelineState* CreatePSO(const PipelineStateDesc& pipelineState);

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

    VkDevice GetVKDevice() const { return device; }

    const std::vector<ConstantBuffer*>& GetFrameConstantBuffers() const { return frameConstantBuffers; }

private:
    VkCommandBuffer BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

    void CreateDescriptorPool();
    void DestroyDescriptorPool();

    void CreatePersistentVertexBuffer();
    void DestroyPersistentVertexBuffer();

    VkShaderModule CreateShaderModule(const std::vector<uint8_t>& code) const;

    DescriptorSetLayoutDesc CreateDescriptorSetLayoutDesc() const;

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

private:
    Renderer* renderer = nullptr;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    VkQueue graphicsQueue = VK_NULL_HANDLE;

    VkCommandPool commandPool = VK_NULL_HANDLE;

    std::vector<ConstantBuffer*> frameConstantBuffers;

    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

public:
    VkBuffer persistentVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory persistentVertexBufferMemory = VK_NULL_HANDLE;
    void* mappedPersistentVertexBufferData = nullptr;
    uint32_t vertexCount = 0;
};

} // namespace URay::Render
