#include "RenderDevice.h"

#include "Render/GPUResourceManager.h"
#include "Render/Renderer.h"
#include "Render/RenderInfo.h"
#include "Render/RHI/Buffer/ConstantBuffer.h"
#include "Render/RHI/Buffer/IndexBuffer.h"
#include "Render/RHI/Buffer/MeshBuffer.h"
#include "Render/RHI/Buffer/VertexBuffer.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/CommandBuffer/CommandPool.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/Descriptor/DescriptorSetLayout.h"
#include "Render/RHI/Descriptor/DescriptorSetLayoutBuilder.h"
#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/Framebuffer.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineLayout/PipelineLayoutDesc.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/PipelineState/PipelineStateDesc.h"
#include "Render/RHI/SwapChain.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureDesc.h"
#include "Render/RHI/Texture/TextureSampler.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/RHI/Vulkan/VulkanContext.h"
#include "Render/RHI/Vulkan/VulkanSurfaceSupport.h"
#include "Render/RHI/Vulkan/VulkanUtils.h"
#include "Render/Shader/Shader.h"

#include "Core/Type/Types.h"

#include <cassert>
#include <map>
#include <set>
#include <stdexcept>

namespace URay::Render
{

RenderDevice::RenderDevice(VulkanContext& context)
    : context(context)
{
}

RenderDevice::~RenderDevice() = default;

bool RenderDevice::Initialize()
{
    instance = context.GetInstance();

    if (!PickPhysicalDevice())
        return false;
    if (!CreateLogicalDevice())
        return false;

    commandPool.reset(CreateCommandPool(QueueType::Graphics, CommandPoolFlags::Transient));
    if (!commandPool)
        return false;

    CreateDescriptorPool();

    return true;
}

void RenderDevice::Finalize()
{
    DestroyDescriptorPool();

    if (commandPool)
    {
        commandPool.reset();
    }

    if (device)
    {
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }
}

VertexBuffer* RenderDevice::CreateVertexBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties)
{
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    CreateBuffer(size, usage, properties, handle, memory);

    VertexBuffer* vertexBuffer = new VertexBuffer(device, size, handle, memory);
    return vertexBuffer;
}

VertexBuffer* RenderDevice::CreateVertexBuffer(const std::vector<VertexPNT>& vertices)
{
    VkDeviceSize bufferSize = sizeof(VertexPNT) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    std::memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(device, stagingBufferMemory);

    VertexBuffer* vertexBuffer = CreateVertexBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    CopyBuffer(stagingBuffer, vertexBuffer->GetHandle(), bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    return vertexBuffer;
}

IndexBuffer* RenderDevice::CreateIndexBuffer(const std::vector<uint32>& indices)
{
    VkDeviceSize bufferSize = sizeof(uint32) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    std::memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(device, stagingBufferMemory);

    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    CreateBuffer(bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 handle, memory);

    CopyBuffer(stagingBuffer, handle, bufferSize);

    IndexBuffer* indexBuffer = new IndexBuffer(device, bufferSize, handle, memory);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    return indexBuffer;
}

MeshBuffer* RenderDevice::CreateMeshBuffer(VertexBuffer* inVertexBuffer, IndexBuffer* inIndexBuffer)
{
    return new MeshBuffer(device, inVertexBuffer, inIndexBuffer);
}

Texture* RenderDevice::CreateTexture(const TextureDesc& desc)
{
    if (desc.width == 0 || desc.height == 0)
        return nullptr;
    if (desc.format == Format::Unknown)
        return nullptr;
    if (desc.usage == TextureUsage::None)
        return nullptr;

    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory imageMemory = VK_NULL_HANDLE;

    VkFormat format = Vulkan::ToVkFormat(desc.format);
    VkImageUsageFlags usageFlags = Vulkan::ToVkImageUsageFlags(desc.usage);

    if (!CreateImage(desc.width, desc.height,
                     format, VK_IMAGE_TILING_OPTIMAL,
                     usageFlags,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     image, imageMemory))
    {
        return nullptr;
    }

    Texture* newTexture = new Texture(device, image, imageMemory, desc);
    return newTexture;
}

bool RenderDevice::UploadTextureData(Texture* texture, std::span<const uint8> pixelData)
{
    if (!texture)
        return false;

    const TextureDesc& textureDesc = texture->GetDesc();

    if ((textureDesc.usage & TextureUsage::TransferDst) == TextureUsage::None)
        return false;

    VkFormat vkFormat = Vulkan::ToVkFormat(textureDesc.format);

    VkDeviceSize dataSize = static_cast<VkDeviceSize>(textureDesc.width) * textureDesc.height * 4;
    if (dataSize != static_cast<VkDeviceSize>(pixelData.size()))
        return false;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(dataSize,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer,
                 stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, dataSize, 0, &data);
    std::memcpy(data, pixelData.data(), static_cast<size_t>(dataSize));
    vkUnmapMemory(device, stagingBufferMemory);

    TransitionImageLayout(
        texture->GetHandle(), vkFormat,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(
        stagingBuffer, texture->GetHandle(),
        textureDesc.width, textureDesc.height);
    TransitionImageLayout(
        texture->GetHandle(), vkFormat,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    return true;
}

TextureView* RenderDevice::CreateTextureView(Texture* texture)
{
    if (!texture)
        return nullptr;

    const TextureDesc& textureDesc = texture->GetDesc();
    VkFormat vkFormat = Vulkan::ToVkFormat(textureDesc.format);
    VkImageAspectFlags vkAspectFlags = Vulkan::ToVkImageAspectFlags(textureDesc.format);

    VkImageView imageView = CreateImageView(texture->GetHandle(), vkFormat, vkAspectFlags);

    if (imageView == VK_NULL_HANDLE)
        return nullptr;

    TextureView* textureView = new TextureView(device, imageView, texture);
    return textureView;
}

VkSampler RenderDevice::CreateTextureSampler(const TextureSamplerDesc& samplerDesc)
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = samplerDesc.magFilter;
    samplerInfo.minFilter = samplerDesc.minFilter;
    samplerInfo.addressModeU = samplerDesc.addressModeU;
    samplerInfo.addressModeV = samplerDesc.addressModeV;
    samplerInfo.addressModeW = samplerDesc.addressModeW;
    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkSampler sampler = VK_NULL_HANDLE;

    if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
        return VK_NULL_HANDLE;

    return sampler;
}

DescriptorSetLayout* RenderDevice::CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc)
{
    DescriptorSetLayoutBuilder builder;

    for (const ResourceBinding& binding : desc.bindings)
    {
        builder.AddBinding(binding);
    }

    return builder.Build(device);
}

DescriptorSet* RenderDevice::CreateDescriptorSet(DescriptorSetLayout* layout)
{
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;

    VkDescriptorSetLayout vkLayout = layout->GetHandle();
    allocInfo.pSetLayouts = &vkLayout;

    VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
    if (vkAllocateDescriptorSets(device, &allocInfo, &vkDescriptorSet) != VK_SUCCESS)
        return nullptr;

    DescriptorSet* descriptorSet = new DescriptorSet(device, vkDescriptorSet, layout);
    return descriptorSet;
}

PipelineLayout* RenderDevice::CreatePipelineLayout(const PipelineLayoutDesc& desc)
{
    uint32 maxSetNum = 0;
    for (auto& [set, layout] : desc.setLayouts)
    {
        maxSetNum = std::max(maxSetNum, set);
    }

    std::vector<VkDescriptorSetLayout> setLayouts(maxSetNum + 1, VK_NULL_HANDLE);
    for (auto& [set, layout] : desc.setLayouts)
    {
        setLayouts[set] = layout->GetHandle();
    }

    std::vector<VkPushConstantRange> pushConstantRanges(desc.pushConstantRanges.size());
    for (size_t i = 0; i < desc.pushConstantRanges.size(); ++i)
    {
        pushConstantRanges[i].offset = desc.pushConstantRanges[i].offset;
        pushConstantRanges[i].size = desc.pushConstantRanges[i].size;
        pushConstantRanges[i].stageFlags = ToVkShaderStageFlags(desc.pushConstantRanges[i].stages);
    }

    VkPipelineLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = static_cast<uint32>(setLayouts.size());
    layoutInfo.pSetLayouts = setLayouts.data();
    layoutInfo.pushConstantRangeCount = static_cast<uint32>(pushConstantRanges.size());
    layoutInfo.pPushConstantRanges = pushConstantRanges.data();

    VkPipelineLayout handle = VK_NULL_HANDLE;
    if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &handle) != VK_SUCCESS)
        return nullptr;

    PipelineLayout* pipelineLayout = new PipelineLayout(device, handle);
    pipelineLayout->SetSupportPushConstants(pushConstantRanges.size() > 0);

    return pipelineLayout;
}

PipelineState* RenderDevice::CreatePSO(const PipelineStateDesc& desc, PipelineLayout& layout, VkRenderPass renderPass)
{
    VkShaderModule vertShaderModule = CreateShaderModule(desc.shader->GetVertexStage().code);
    VkShaderModule fragShaderModule = CreateShaderModule(desc.shader->GetFragmentStage().code);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "VSMain";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "PSMain";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    const auto bindingDescription = desc.vertexLayout == VertexLayout::PNT
                                        ? VertexPNT::GetBindingDescription()
                                        : Vertex::GetBindingDescription();
    const auto attributeDescriptions = desc.vertexLayout == VertexLayout::PNT
                                           ? VertexPNT::GetAttributeDescriptions()
                                           : Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    switch (desc.topology)
    {
    case PrimitiveTopology::TriangleList:
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        break;
    case PrimitiveTopology::LineList:
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        break;
    }
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    VkRect2D scissor = {};

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    switch (desc.rasterizer.cullMode)
    {
    case CullMode::Front:
        rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
        break;
    case CullMode::Back:
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        break;
    case CullMode::None:
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        break;
    }
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = desc.depthStencil.depthTestEnable ? VK_TRUE : VK_FALSE;
    depthStencil.depthWriteEnable = desc.depthStencil.depthWriteEnable ? VK_TRUE : VK_FALSE;
    switch (desc.depthStencil.depthCompareOp)
    {
    case CompareOp::Less:
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        break;
    case CompareOp::Equal:
        depthStencil.depthCompareOp = VK_COMPARE_OP_EQUAL;
        break;
    }
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f;
    depthStencil.maxDepthBounds = 1.0f;
    depthStencil.stencilTestEnable = desc.depthStencil.stencilTestEnable ? VK_TRUE : VK_FALSE;
    depthStencil.front = {};
    depthStencil.back = {};

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                          VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT |
                                          VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = desc.blend.blendEnable ? VK_TRUE : VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = layout.GetHandle();
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
    {
        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
        return VK_NULL_HANDLE;
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);

    PipelineState* pso = new PipelineState(device, pipeline, &layout);

    return pso;
}

Framebuffer* RenderDevice::CreateFramebuffer(const FramebufferDesc& desc)
{
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = desc.renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32>(desc.attachments.size());
    framebufferInfo.pAttachments = desc.attachments.data();
    framebufferInfo.width = desc.extent.width;
    framebufferInfo.height = desc.extent.height;
    framebufferInfo.layers = 1;

    VkFramebuffer handle = VK_NULL_HANDLE;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &handle) != VK_SUCCESS)
        return nullptr;

    Framebuffer* framebuffer = new Framebuffer(device, handle);
    return framebuffer;
}

SwapChain* RenderDevice::CreateSwapChain(const SwapChainDesc& desc)
{
    SwapChain* swapChain = new SwapChain(*this, context);
    if (!swapChain->Initialize(desc))
    {
        delete swapChain;
        return nullptr;
    }

    return swapChain;
}

CommandPool* RenderDevice::CreateCommandPool(QueueType queueType, CommandPoolFlags poolFlags)
{
    const QueueFamilyIndices indices = FindQueueFamilyIndices(physicalDevice);
    uint32 queueFamilyIndex = 0;

    switch (queueType)
    {
    case QueueType::Graphics:
        queueFamilyIndex = indices.graphicsFamily.value();
        break;
    default:
        break;
    }

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = static_cast<uint32>(poolFlags);
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    VkCommandPool handle = VK_NULL_HANDLE;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &handle) != VK_SUCCESS)
        return nullptr;

    CommandPool* commandPool = new CommandPool(*this, handle);
    return commandPool;
}

void RenderDevice::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties,
                                VkBuffer& buffer, VkDeviceMemory& bufferMemory) const
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        return;

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        return;

    if (vkBindBufferMemory(device, buffer, bufferMemory, 0) != VK_SUCCESS)
        return;
}

void RenderDevice::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const
{
    CommandBuffer* commandBuffer = BeginSingleTimeCommands();

    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer->GetHandle(), srcBuffer, dstBuffer, 1, &copyRegion);

    EndSingleTimeCommands(commandBuffer);
}

bool RenderDevice::CreateImage(uint32 width, uint32 height,
                               VkFormat format, VkImageTiling tiling,
                               VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                               VkImage& image, VkDeviceMemory& imageMemory) const
{
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        return false;

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        vkDestroyImage(device, image, nullptr);
        image = VK_NULL_HANDLE;
        return false;
    }

    if (vkBindImageMemory(device, image, imageMemory, 0) != VK_SUCCESS)
    {
        vkFreeMemory(device, imageMemory, nullptr);
        vkDestroyImage(device, image, nullptr);
        imageMemory = VK_NULL_HANDLE;
        image = VK_NULL_HANDLE;
        return false;
    }

    return true;
}

void RenderDevice::TransitionImageLayout(VkImage image, VkFormat format,
                                         VkImageLayout oldLayout, VkImageLayout newLayout) const
{
    CommandBuffer* commandBuffer = BeginSingleTimeCommands();

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    else
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    vkCmdPipelineBarrier(commandBuffer->GetHandle(), sourceStage, destinationStage,
                         0, 0, nullptr, 0, nullptr, 1, &barrier);

    EndSingleTimeCommands(commandBuffer);
}

void RenderDevice::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height) const
{
    CommandBuffer* commandBuffer = BeginSingleTimeCommands();

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(commandBuffer->GetHandle(), buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndSingleTimeCommands(commandBuffer);
}

VkImageView RenderDevice::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        return VK_NULL_HANDLE;

    return imageView;
}

bool RenderDevice::PickPhysicalDevice()
{
    uint32 deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0)
        return false;

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
        return false;

    return true;
}

bool RenderDevice::CreateLogicalDevice()
{
    QueueFamilyIndices indices = FindQueueFamilyIndices(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32> uniqueQueueFamilies = {
        indices.graphicsFamily.value(),
        indices.presentFamily.value(),
    };

    float queuePriority = 1.0f;
    for (uint32 queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    const auto& deviceExtensions = context.GetDeviceExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
        return false;

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

    return true;
}

bool RenderDevice::IsDeviceSuitable(VkPhysicalDevice device) const
{
    QueueFamilyIndices indices = FindQueueFamilyIndices(device);

    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        Vulkan::SurfaceSupportDetails swapChainSupport = Vulkan::QuerySurfaceSupport(device, context.GetSurface());
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

QueueFamilyIndices RenderDevice::FindQueueFamilyIndices(VkPhysicalDevice device) const
{
    QueueFamilyIndices indices = {};

    uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int index = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = index;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, index, context.GetSurface(), &presentSupport);

        if (presentSupport)
        {
            indices.presentFamily = index;
        }

        if (indices.IsComplete())
            break;

        ++index;
    }

    return indices;
}

bool RenderDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device) const
{
    uint32 extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    const auto& deviceExtensions = context.GetDeviceExtensions();
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

CommandBuffer* RenderDevice::BeginSingleTimeCommands() const
{
    CommandBuffer* commandBuffer = commandPool->Allocate();

    if (!commandBuffer->Begin(CommandBufferUsage::OneTimeSubmit))
    {
        delete commandBuffer;
        return nullptr;
    }

    return commandBuffer;
}

void RenderDevice::EndSingleTimeCommands(CommandBuffer* commandBuffer) const
{
    if (!commandBuffer->End())
        return;

    VkCommandBuffer vkCommandBuffer = commandBuffer->GetHandle();

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    delete commandBuffer;
}

void RenderDevice::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 3> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32>(MAX_FRAMES_IN_FLIGHT) * 1000;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    poolSizes[1].descriptorCount = static_cast<uint32>(MAX_FRAMES_IN_FLIGHT) * 1000;
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLER;
    poolSizes[2].descriptorCount = static_cast<uint32>(MAX_FRAMES_IN_FLIGHT) * 1000;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32>(MAX_FRAMES_IN_FLIGHT) * 1000;

    vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
}

void RenderDevice::DestroyDescriptorPool()
{
    if (descriptorPool)
    {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    }
}

VkShaderModule RenderDevice::CreateShaderModule(const std::vector<uint8>& code) const
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        return VK_NULL_HANDLE;

    return shaderModule;
}

uint32 RenderDevice::FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32 i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if (typeFilter & (1 << i) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    return UINT32_MAX;
}

} // namespace URay::Render
