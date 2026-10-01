#include "UIBatcher.h"

#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/Buffer/BufferDesc.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/Descriptor/DescriptorSetLayout.h"
#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/Device.h"
#include "Render/Rendering/Batch/UIDrawContext.h"
#include "Render/Rendering/Font/FontAtlas.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Engine.h"

#include <vulkan/vulkan.h>

#include <cstring>

namespace URay::Render
{

UIBatcher::UIBatcher(Device& device, ResourceManager& resourceManager)
    : device(device), resourceManager(resourceManager) {}

UIBatcher::~UIBatcher() = default;

bool UIBatcher::Initialize()
{
    const VertexBufferDesc vbDesc = {
        .size = 1024 * 1024 * 4,
        .memoryUsage = MemoryUsage::CpuToGpu
    };
    vertexBuffer.reset(device.CreateVertexBuffer(vbDesc));

    const IndexBufferDesc ibDesc = {
        .size = 1024 * 1024 * 4,
        .memoryUsage = MemoryUsage::CpuToGpu
    };
    indexBuffer.reset(device.CreateIndexBuffer(ibDesc));

    mappedVertexData = vertexBuffer->Map();
    mappedIndexData = indexBuffer->Map();

    return true;
}

void UIBatcher::Finalize()
{
    descriptorSets.clear();
    whiteTextureView = nullptr;
    textureSetLayout = nullptr;
    sampler = VK_NULL_HANDLE;
    shader = nullptr;

    indexBuffer->Unmap();
    vertexBuffer->Unmap();

    indexBuffer.reset();
    vertexBuffer.reset();
}

std::vector<DrawCommand> UIBatcher::Flush(const UIDrawContext& context)
{
    const std::vector<UIDrawBatch>& batches = context.GetBatches();
    const std::vector<VertexUI>& vertices = context.GetVertices();
    const std::vector<uint32>& indices = context.GetIndices();

    if (vertices.empty())
        return {};

    EnsureResources();

    std::vector<DrawCommand> ret;

    VkDeviceSize vertexSize = sizeof(VertexUI) * vertices.size();
    std::memcpy(mappedVertexData, vertices.data(), vertexSize);

    VkDeviceSize indexSize = sizeof(uint32) * indices.size();
    std::memcpy(mappedIndexData, indices.data(), indexSize);

    for (const UIDrawBatch& batch : batches)
    {
        DrawCommand cmd = {};
        cmd.passId = RenderPassId::UI;
        cmd.worldMatrix = Matrix::Identity;
        cmd.vertexBuffer = vertexBuffer.get();
        cmd.vertexCount = static_cast<uint32>(vertices.size());
        cmd.indexBuffer = indexBuffer.get();
        cmd.indexOffset = batch.indexOffset;
        cmd.indexCount = batch.indexCount;

        DescriptorSet* descriptorSet = GetOrCreateDescriptorSet(batch.fontAtlas);
        if (!descriptorSet)
            continue;

        cmd.descriptorSets[1] = descriptorSet;

        PipelineStateDesc psoDesc = {};
        psoDesc.shader = shader;
        psoDesc.topology = PrimitiveTopology::TriangleList;
        psoDesc.vertexLayout = VertexLayout::UI;
        psoDesc.depthStencil.depthTestEnable = false;
        psoDesc.depthStencil.depthWriteEnable = false;
        psoDesc.rasterizer.cullMode = CullMode::None;
        psoDesc.blend.mode = BlendMode::AlphaBlend;

        cmd.pipelineState = psoDesc;

        cmd.scissor = batch.clipRect;

        ret.push_back(cmd);
    }

    return ret;
}

void UIBatcher::EnsureResources()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    if (!shader)
    {
        URay::Shader* shaderAsset = assetDatabase.Find<URay::Shader>(EngineAsset::UIShader);
        shader = resourceManager.GetOrCreateShader(shaderAsset, {});
    }

    if (!textureSetLayout)
    {
        const DescriptorSetLayoutDesc* layoutDesc = shader->GetLayoutDescription(1);
        textureSetLayout = resourceManager.GetOrCreateDescriptorSetLayout(*layoutDesc);
    }

    if (sampler == VK_NULL_HANDLE)
    {
        sampler = resourceManager.GetOrCreateSampler(SamplerDesc{});
    }

    if (!whiteTextureView)
    {
        URay::Texture* whiteTextureAsset = assetDatabase.Find<URay::Texture>(EngineAsset::WhiteTexture);
        Texture* whiteTexture = resourceManager.GetOrCreateTexture(whiteTextureAsset);
        whiteTextureView = resourceManager.GetOrCreateTextureView(whiteTexture, TextureViewDesc{});
    }
}

DescriptorSet* UIBatcher::GetOrCreateDescriptorSet(const FontAtlas* fontAtlas)
{
    const auto it = descriptorSets.find(fontAtlas);
    if (it != descriptorSets.end())
        return it->second.get();

    TextureView* view = fontAtlas ? fontAtlas->GetTextureView() : whiteTextureView;
    if (!view)
        return nullptr;

    std::unique_ptr<DescriptorSet> descriptorSet = nullptr;
    descriptorSet.reset(device.CreateDescriptorSet(textureSetLayout));
    if (!descriptorSet)
        return nullptr;

    descriptorSet->WriteSampledImage(0, view);
    descriptorSet->WriteSampler(1, sampler);

    DescriptorSet* result = descriptorSet.get();
    descriptorSets.emplace(fontAtlas, std::move(descriptorSet));

    return result;
}

} // namespace URay::Render
