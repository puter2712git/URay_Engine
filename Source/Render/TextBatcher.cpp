#include "TextBatcher.h"

#include "Render/Descriptor/DescriptorSet.h"
#include "Render/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/DrawCommand/DrawCommand.h"
#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/GPUResourceManager.h"
#include "Render/RenderDevice.h"
#include "Render/Renderer.h"
#include "Render/Shader/Shader.h"
#include "Render/Shader/ShaderManager.h"

#include "Engine/Font/FontAsset.h"

namespace URay
{

TextBatcher::TextBatcher(Renderer* renderer)
    : renderer(renderer)
{
    RenderDevice* device = renderer->GetDevice();

    VkDeviceSize bufferSize = 1024 * 1024 * 4;

    device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         vertexBuffer, vertexBufferMemory);

    vkMapMemory(device->GetVKDevice(), vertexBufferMemory, 0, bufferSize, 0, &mappedVertexBufferData);

    Shader* shader = renderer->GetShaderManager()->GetOrCreate("shader");

    const DescriptorSetLayoutDesc* layoutDesc = shader->GetDescriptorSetLayoutDesc(1);

    GPUResourceManager* resourceManager = renderer->GetResourceManager();
    DescriptorSetLayout* setLayout = resourceManager->GetOrCreateDescriptorSetLayout(*layoutDesc);
    descriptorSet = device->CreateDescriptorSet(setLayout);
}

TextBatcher::~TextBatcher()
{
    RenderDevice* device = renderer->GetDevice();

    vkDestroyBuffer(device->GetVKDevice(), vertexBuffer, nullptr);
    vkFreeMemory(device->GetVKDevice(), vertexBufferMemory, nullptr);
}

void TextBatcher::Reset()
{
    for (auto& [font, verts] : vertices)
    {
        verts.clear();
    }
}

void TextBatcher::Flush(DrawCommandBuilder& builder)
{
    for (auto& [font, verts] : vertices)
    {
        if (verts.empty())
            continue;

        VkDeviceSize size = sizeof(Vertex) * verts.size();
        std::memcpy(mappedVertexBufferData, verts.data(), size);

        DrawCommand cmd = {};
        cmd.worldMatrix = Matrix::Identity;
        cmd.vertexBuffer = vertexBuffer;
        cmd.vertexCount = static_cast<uint32_t>(verts.size());

        PipelineStateDesc psoDesc = {};
        psoDesc.shader = renderer->GetShaderManager()->GetOrCreate("default");
        psoDesc.topology = PrimitiveTopology::TriangleList;
        psoDesc.depthStencil.depthTestEnable = true;
        psoDesc.depthStencil.depthWriteEnable = true;
        psoDesc.rasterizer.cullMode = CullMode::None;

        cmd.pipelineState = psoDesc;

        GPUResourceManager* resourceManager = renderer->GetResourceManager();
        Texture* texture = resourceManager->GetOrCreateTexture(font->GetFilePath());
        TextureView* textureView = resourceManager->GetOrCreateTextureView(texture);

        descriptorSet->WriteSampledImage(0, textureView);
        descriptorSet->WriteSampler(1, resourceManager->GetOrCreateTextureSampler({}));

        cmd.descriptorSet = descriptorSet;

        builder.AddDrawCommand(cmd);
    }
}

void TextBatcher::Collect(const TextCommandContext& context)
{
    if (!context.font)
        return;

    auto it = vertices.find(context.font);
    if (it == vertices.end())
    {
        vertices.insert({ context.font, std::vector<Vertex>() });
    }

    std::vector<Vertex>& verts = vertices[context.font];

    for (size_t i = 0; i < context.text.length(); ++i)
    {
        const Vector3 p0 = Vector3::Zero;
        const Vector3 p1 = Vector3(context.font->GetCellWidth(), 0, 0);
        const Vector3 p2 = Vector3(context.font->GetCellWidth(), 0, context.font->GetCellHeight());
        const Vector3 p3 = Vector3(0, 0, context.font->GetCellHeight());
    }
}

} // namespace URay
