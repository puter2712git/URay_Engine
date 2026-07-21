#include "DrawCommandBuilder.h"

#include "Render/Material/Material.h"
#include "Render/Renderer.h"

namespace URay
{

DrawCommandBuilder::DrawCommandBuilder(Renderer& renderer)
    : renderer(renderer)
{
}

void DrawCommandBuilder::Reset()
{
    drawCmds.clear();
}

void DrawCommandBuilder::FlushLines()
{
    VkDeviceSize lineDataSize = sizeof(Vertex) * lineVertices.size();
    std::memcpy(renderer.mappedVertexBufferData, lineVertices.data(), lineDataSize);
    renderer.vertexCount = static_cast<uint32_t>(lineVertices.size());

    lineVertices.clear();
}

void DrawCommandBuilder::BuildFromMesh(const MeshCommandContext& context)
{
    DrawCommand cmd = {};
    cmd.worldMatrix = context.worldMatrix;
    cmd.vertexBuffer = context.vertexBuffer;
    cmd.indexBuffer = context.indexBuffer;
    cmd.indexCount = context.indexCount;

    PipelineState state = {};
    state.shader = context.material->GetShader();

    cmd.pipelineState = state;

    drawCmds.push_back(cmd);
}

void DrawCommandBuilder::BuildFromLine(const LineCommandContext& context)
{
    Vertex v0 = {};
    v0.pos = context.start;
    v0.color = context.color;

    Vertex v1 = {};
    v1.pos = context.end;
    v1.color = context.color;

    lineVertices.push_back(v0);
    lineVertices.push_back(v1);
}

} // namespace URay
