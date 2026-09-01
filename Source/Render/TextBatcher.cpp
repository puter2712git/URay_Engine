#include "TextBatcher.h"

#include "Render/DrawCommand/DrawCommandContext.h"
#include "Render/GPUResourceManager.h"
#include "Render/Renderer.h"
#include "Render/RHI/Buffer/VertexBuffer.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/Shader/Shader.h"
#include "Render/Shader/ShaderManager.h"

#include "Core/Type/Types.h"

#include "Engine/Asset/Font/Font.h"

#include <cstring>

namespace URay::Render
{

TextBatcher::TextBatcher(RenderDevice& device, GPUResourceManager& resourceManager, ShaderManager& shaderManager)
    : device(device), resourceManager(resourceManager), shaderManager(shaderManager)
{
}

TextBatcher::~TextBatcher() = default;

bool TextBatcher::Initialize()
{
    VkDeviceSize bufferSize = 1024 * 1024 * 4;

    vertexBuffer.reset(device.CreateVertexBuffer(
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

    mappedVertexBufferData = vertexBuffer->Map();

    Shader* shader = shaderManager.GetOrCreate("Font");

    const DescriptorSetLayoutDesc* layoutDesc = shader->GetDescriptorSetLayoutDesc(1);

    DescriptorSetLayout* setLayout = resourceManager.GetOrCreateDescriptorSetLayout(*layoutDesc);
    descriptorSet = device.CreateDescriptorSet(setLayout);

    return true;
}

void TextBatcher::Finalize()
{
    if (vertexBuffer)
    {
        if (mappedVertexBufferData)
        {
            vertexBuffer->Unmap();
        }

        vertexBuffer.reset();
    }
}

void TextBatcher::Reset()
{
    for (auto& [font, verts] : vertices)
    {
        verts.clear();
    }
}

std::vector<DrawCommand> TextBatcher::Flush()
{
    std::vector<DrawCommand> drawCmds;

    for (auto& [font, verts] : vertices)
    {
        if (verts.empty())
            continue;

        VkDeviceSize size = sizeof(Vertex) * verts.size();
        std::memcpy(mappedVertexBufferData, verts.data(), size);

        DrawCommand cmd = {};
        cmd.worldMatrix = Matrix::Identity;
        cmd.vertexBuffer = vertexBuffer.get();
        cmd.vertexCount = static_cast<uint32>(verts.size());

        PipelineStateDesc psoDesc = {};
        psoDesc.shader = shaderManager.GetOrCreate("Font");
        psoDesc.topology = PrimitiveTopology::TriangleList;
        psoDesc.depthStencil.depthTestEnable = true;
        psoDesc.depthStencil.depthWriteEnable = true;
        psoDesc.rasterizer.cullMode = CullMode::None;
        psoDesc.blend.blendEnable = true;

        cmd.pipelineState = psoDesc;

        Texture* texture = resourceManager.GetOrCreateTexture(font->GetBitmapTexture());
        TextureView* textureView = resourceManager.GetOrCreateTextureView(texture);

        descriptorSet->WriteSampledImage(0, textureView);
        descriptorSet->WriteSampler(1, resourceManager.GetOrCreateTextureSampler({}));

        cmd.descriptorSet = descriptorSet;

        drawCmds.push_back(cmd);
    }

    return drawCmds;
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
