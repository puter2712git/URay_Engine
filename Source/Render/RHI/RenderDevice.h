#pragma once

#include "Render/Vertex.h"

#include <spirv/spirv_reflect.h>
#include <vulkan/vulkan.h>

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace URay
{
class Texture;
}

namespace URay::Render
{

class VulkanContext;
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
class Framebuffer;
class SwapChain;
class CommandPool;
class CommandBuffer;

struct TextureDesc;
struct TextureSamplerDesc;
struct DescriptorSetLayoutDesc;
struct PipelineLayoutDesc;
struct PipelineStateDesc;
struct FramebufferDesc;
struct SwapChainDesc;

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class RenderDevice
{
public:
    RenderDevice(VulkanContext& context);
    ~RenderDevice();

public:
    bool Initialize();
    void Finalize();

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
    PipelineState* CreatePSO(const PipelineStateDesc& pipelineState, PipelineLayout& layout, VkRenderPass renderPass);

    Framebuffer* CreateFramebuffer(const FramebufferDesc& desc);
    SwapChain* CreateSwapChain(const SwapChainDesc& desc);

    CommandPool* CreateCommandPool();

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

    QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice device) const;

    VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
    VkDevice GetVKDevice() const { return device; }

    const std::vector<ConstantBuffer*>& GetFrameConstantBuffers() const { return frameConstantBuffers; }

    VkQueue GetGraphicsQueue() const { return graphicsQueue; }
    VkQueue GetPresentQueue() const { return presentQueue; }

private:
    bool PickPhysicalDevice();
    bool CreateLogicalDevice();

    bool IsDeviceSuitable(VkPhysicalDevice device) const;
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;

    CommandBuffer* BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(CommandBuffer* commandBuffer) const;

    void CreateDescriptorPool();
    void DestroyDescriptorPool();

    VkShaderModule CreateShaderModule(const std::vector<uint8_t>& code) const;

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

private:
    VulkanContext& context;

    Renderer* renderer = nullptr;

    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;

    std::unique_ptr<CommandPool> commandPool = nullptr;

    std::vector<ConstantBuffer*> frameConstantBuffers;

    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
};

} // namespace URay::Render
