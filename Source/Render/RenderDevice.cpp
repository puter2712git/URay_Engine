#include "RenderDevice.h"

#include "ConstantBuffer.h"
#include "Descriptor/DescriptorSet.h"
#include "Descriptor/DescriptorSetLayout.h"
#include "Descriptor/DescriptorSetLayoutBuilder.h"
#include "Descriptor/DescriptorSetLayoutDesc.h"
#include "GPUResourceManager.h"
#include "IndexBuffer.h"
#include "Mesh.h"
#include "PipelineLayout/PipelineLayout.h"
#include "PipelineLayout/PipelineLayoutDesc.h"
#include "PipelineState/PipelineState.h"
#include "PipelineState/PipelineStateDesc.h"
#include "RenderInfo.h"
#include "Renderer.h"
#include "Shader/Shader.h"
#include "Texture/Texture.h"
#include "Texture/TextureSampler.h"
#include "Texture/TextureView.h"
#include "VertexBuffer.h"

#include "Engine/Texture/Texture.h"

#include <stb/stb_image.h>

#include <cassert>
#include <map>
#include <stdexcept>

namespace URay::RHI
{

RenderDevice::RenderDevice(Renderer* renderer,
                           VkPhysicalDevice physicalDevice, VkDevice device,
                           VkQueue graphicsQueue, VkCommandPool commandPool)
    : renderer(renderer),
      physicalDevice(physicalDevice), device(device),
      graphicsQueue(graphicsQueue), commandPool(commandPool)
{
    VkDeviceSize frameBufferSize = sizeof(FrameConstants);
    frameConstantBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkBuffer handle;
        VkDeviceMemory memory;

        CreateBuffer(frameBufferSize,
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     handle, memory);

        frameConstantBuffers[i] = new ConstantBuffer(device, handle, memory, frameBufferSize);
    }

    CreateDescriptorPool();

    CreatePersistentVertexBuffer();
}

RenderDevice::~RenderDevice()
{
    DestroyPersistentVertexBuffer();

    DestroyDescriptorPool();

    for (ConstantBuffer* buffer : frameConstantBuffers)
    {
        delete buffer;
        buffer = nullptr;
    }
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

    VertexBuffer* vertexBuffer = new VertexBuffer(device, bufferSize);

    CreateBuffer(bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 vertexBuffer->GetBufferRef(), vertexBuffer->GetMemoryRef());

    CopyBuffer(stagingBuffer, vertexBuffer->GetBufferRef(), bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    return vertexBuffer;
}

IndexBuffer* RenderDevice::CreateIndexBuffer(const std::vector<uint32_t>& indices)
{
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    std::memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(device, stagingBufferMemory);

    IndexBuffer* indexBuffer = new IndexBuffer(device, bufferSize);

    CreateBuffer(bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 indexBuffer->GetBufferRef(), indexBuffer->GetMemoryRef());

    CopyBuffer(stagingBuffer, indexBuffer->GetBufferRef(), bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    return indexBuffer;
}

Mesh* RenderDevice::CreateMesh(VertexBuffer* inVertexBuffer, IndexBuffer* inIndexBuffer)
{
    Mesh* mesh = new Mesh(device, inVertexBuffer, inIndexBuffer);
    return mesh;
}

Texture* RenderDevice::CreateTexture(const ::URay::Texture* textureAsset)
{
    stbi_set_flip_vertically_on_load(true);

    VkDeviceSize size = textureAsset->GetPixels().size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(size,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer,
                 stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, size, 0, &data);
    std::memcpy(data, textureAsset->GetPixels().data(), static_cast<size_t>(size));
    vkUnmapMemory(device, stagingBufferMemory);

    VkImage image;
    VkDeviceMemory imageMemory;

    if (!CreateImage(textureAsset->GetWidth(), textureAsset->GetHeight(),
                     VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                     VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     image, imageMemory))
    {
        return nullptr;
    }

    TransitionImageLayout(
        image, VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(
        stagingBuffer, image,
        static_cast<uint32_t>(textureAsset->GetWidth()), static_cast<uint32_t>(textureAsset->GetHeight()));
    TransitionImageLayout(
        image, VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    Texture* texture = new Texture(device, image, imageMemory);
    return texture;
}

TextureView* RenderDevice::CreateTextureView(Texture* texture)
{
    VkImageView imageView = CreateImageView(texture->GetHandle(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

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
    uint32_t maxSetNum = 0;
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
    layoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
    layoutInfo.pSetLayouts = setLayouts.data();
    layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    layoutInfo.pPushConstantRanges = pushConstantRanges.data();

    VkPipelineLayout handle = VK_NULL_HANDLE;
    if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &handle) != VK_SUCCESS)
        return nullptr;

    PipelineLayout* pipelineLayout = new PipelineLayout(device, handle);
    pipelineLayout->SetSupportPushConstants(pushConstantRanges.size() > 0);

    return pipelineLayout;
}

PipelineState* RenderDevice::CreatePSO(const PipelineStateDesc& desc)
{
    PipelineLayoutDesc pipelineLayoutDesc = {};

    GPUResourceManager* resourceManager = renderer->GetResourceManager();
    for (auto& [set, descriptorSetLayoutDesc] : desc.shader->GetDescriptorSetLayoutDescs())
    {
        pipelineLayoutDesc.setLayouts[set] =
            resourceManager->GetOrCreateDescriptorSetLayout(descriptorSetLayoutDesc);
    }

    pipelineLayoutDesc.pushConstantRanges = desc.shader->GetPushConstantRanges();

    PipelineLayout* pipelineLayout = resourceManager->GetOrCreatePipelineLayout(pipelineLayoutDesc);

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
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
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
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
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

    VkExtent2D swapChainExtent = renderer->GetSwapChainExtent();

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(swapChainExtent.height);
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = -static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;

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
    pipelineInfo.layout = pipelineLayout->GetHandle();
    pipelineInfo.renderPass = renderer->GetRenderPass();
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

    PipelineState* pso = new PipelineState(device, pipeline, pipelineLayout);

    return pso;
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
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    EndSingleTimeCommands(commandBuffer);
}

bool RenderDevice::CreateImage(uint32_t width, uint32_t height,
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
        return false;

    if (vkBindImageMemory(device, image, imageMemory, 0) != VK_SUCCESS)
        return false;

    return true;
}

void RenderDevice::TransitionImageLayout(VkImage image, VkFormat format,
                                         VkImageLayout oldLayout, VkImageLayout newLayout) const
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

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

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage,
                         0, 0, nullptr, 0, nullptr, 1, &barrier);

    EndSingleTimeCommands(commandBuffer);
}

void RenderDevice::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

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

    vkCmdCopyBufferToImage(commandBuffer, buffer, image,
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

VkCommandBuffer RenderDevice::BeginSingleTimeCommands() const
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void RenderDevice::EndSingleTimeCommands(VkCommandBuffer commandBuffer) const
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void RenderDevice::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 3> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 1000;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 1000;
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLER;
    poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 1000;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 1000;

    vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
}

void RenderDevice::DestroyDescriptorPool()
{
    if (descriptorPool)
    {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    }
}

void RenderDevice::CreatePersistentVertexBuffer()
{
    VkDeviceSize bufferSize = 1024 * 1024 * 4;

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 persistentVertexBuffer, persistentVertexBufferMemory);

    vkMapMemory(device, persistentVertexBufferMemory, 0, bufferSize, 0, &mappedPersistentVertexBufferData);
}

void RenderDevice::DestroyPersistentVertexBuffer()
{
    vkDestroyBuffer(device, persistentVertexBuffer, nullptr);
    vkFreeMemory(device, persistentVertexBufferMemory, nullptr);
}

VkShaderModule RenderDevice::CreateShaderModule(const std::vector<uint8_t>& code) const
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        return VK_NULL_HANDLE;

    return shaderModule;
}

uint32_t RenderDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if (typeFilter & (1 << i) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    return UINT32_MAX;
}

} // namespace URay::RHI
