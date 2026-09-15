#pragma once

#include "BlendState.h"
#include "DepthStencilState.h"
#include "PrimitiveTopology.h"
#include "RasterizerState.h"
#include "Render/RHI/Texture/TextureDesc.h"
#include "Render/Vertex.h"

#include <functional>
#include <vector>

namespace URay::Render
{

class Shader;

struct PipelineRenderingDesc
{
    std::vector<Format> colorAttachmentFormats;

    Format depthAttachmentFormat = Format::Unknown;
    Format stencilAttachmentFormat = Format::Unknown;

    bool operator==(const PipelineRenderingDesc&) const = default;
};

struct PipelineStateDesc
{
    Shader* shader = nullptr;

    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    VertexLayout vertexLayout = VertexLayout::PTC;

    DepthStencilState depthStencil = {};
    RasterizerState rasterizer = {};
    BlendState blend = {};

    PipelineRenderingDesc rendering = {};

    bool operator==(const PipelineStateDesc&) const = default;
};

struct PipelineStateDescHash
{
    size_t operator()(const PipelineStateDesc& desc) const
    {
        size_t hash = 0;

        auto combine = [&hash](auto value)
        {
            const size_t valueHash = std::hash<decltype(value)>{}(value);
            hash ^= valueHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        };

        combine(desc.shader);
        combine(desc.topology);
        combine(desc.vertexLayout);
        combine(desc.depthStencil.depthTestEnable);
        combine(desc.depthStencil.depthWriteEnable);
        combine(desc.depthStencil.depthCompareOp);
        combine(desc.depthStencil.stencilTestEnable);
        combine(desc.rasterizer.cullMode);
        combine(desc.rasterizer.polygonMode);
        combine(desc.blend.mode);

        for (Format colorAttachmentFormat : desc.rendering.colorAttachmentFormats)
            combine(colorAttachmentFormat);
        combine(desc.rendering.depthAttachmentFormat);
        combine(desc.rendering.stencilAttachmentFormat);

        return hash;
    }
};

} // namespace URay::Render
