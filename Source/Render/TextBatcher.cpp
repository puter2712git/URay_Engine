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

#include "Engine/Font/Font.h"

#include <cstring>

namespace URay::Render
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

    Shader* shader = renderer->GetShaderManager()->GetOrCreate("Font");

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
        psoDesc.shader = renderer->GetShaderManager()->GetOrCreate("Font");
        psoDesc.topology = PrimitiveTopology::TriangleList;
        psoDesc.depthStencil.depthTestEnable = true;
        psoDesc.depthStencil.depthWriteEnable = true;
        psoDesc.rasterizer.cullMode = CullMode::None;
        psoDesc.blend.blendEnable = true;

        cmd.pipelineState = psoDesc;

        GPUResourceManager* resourceManager = renderer->GetResourceManager();
        Texture* texture = resourceManager->GetOrCreateTexture(font->GetBitmapTexture());
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

    const float cellWidth = context.font->GetCellWidth() * 0.01f;
    const float cellHeight = context.font->GetCellHeight() * 0.01f;
    const float advance = context.font->GetCellWidth() * 0.01f;

    for (size_t i = 0; i < context.text.length(); ++i)
    {
        const Vector3 p0 = context.worldMatrix.TransformPoint(Vector3(cellWidth * i, 0.0f, 0.0f));
        const Vector3 p1 = context.worldMatrix.TransformPoint(Vector3(cellWidth * (i + 1), 0, 0));
        const Vector3 p2 = context.worldMatrix.TransformPoint(Vector3(cellWidth * (i + 1), 0, cellHeight));
        const Vector3 p3 = context.worldMatrix.TransformPoint(Vector3(cellWidth * i, 0, cellHeight));

        const Vector2 startUV = context.font->GetUVFromChar(context.text[i]);

        const float vTop = startUV.y;
        const float vBottom = startUV.y + context.font->GetCellHeightUV();

        const Vector2 uv0 = Vector2(startUV.x, vBottom);
        const Vector2 uv1 = Vector2(startUV.x + context.font->GetCellWidthUV(), vBottom);
        const Vector2 uv2 = Vector2(startUV.x + context.font->GetCellWidthUV(), vTop);
        const Vector2 uv3 = Vector2(startUV.x, vTop);

        verts.push_back({ p0, uv0, Color::White });
        verts.push_back({ p1, uv1, Color::White });
        verts.push_back({ p2, uv2, Color::White });

        verts.push_back({ p0, uv0, Color::White });
        verts.push_back({ p2, uv2, Color::White });
        verts.push_back({ p3, uv3, Color::White });
    }
}

} // namespace URay::Render
