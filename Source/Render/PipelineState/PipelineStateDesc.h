#pragma once

#include "BlendState.h"
#include "DepthStencilState.h"
#include "PrimitiveTopology.h"
#include "RasterizerState.h"
#include "Render/Vertex.h"

#include <functional>

namespace URay::RHI
{

class Shader;

struct PipelineStateDesc
{
    Shader* shader = nullptr;

    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    VertexLayout vertexLayout = VertexLayout::PTC;

    DepthStencilState depthStencil = {};
    RasterizerState rasterizer = {};
    BlendState blend = {};

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
        combine(desc.blend.blendEnable);

        return hash;
    }
};

} // namespace URay::RHI
