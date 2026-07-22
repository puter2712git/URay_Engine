#include "DrawCommandBuilder.h"

#include "Render/IndexBuffer.h"
#include "Render/Material/Material.h"
#include "Render/Renderer.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/VertexBuffer/VertexBuffer.h"

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

    DrawCommand cmd = {};
    cmd.worldMatrix = Matrix::Identity;
    cmd.vertexBuffer = renderer.persistentVertexBuffer;
    cmd.vertexCount = static_cast<uint32_t>(lineVertices.size());

    PipelineState state = {};
    state.shader = renderer.GetShaderManager()->GetOrCreate("line");
    state.topology = PrimitiveTopology::LineList;

    cmd.pipelineState = state;

    drawCmds.push_back(cmd);

    lineVertices.clear();
}

void DrawCommandBuilder::BuildFromMesh(const MeshCommandContext& context)
{
    DrawCommand cmd = {};
    cmd.worldMatrix = context.worldMatrix;
    cmd.colorTint = context.colorTint;
    cmd.vertexBuffer = context.vertexBuffer->GetBufferRef();
    cmd.vertexCount = context.vertexCount;
    cmd.indexBuffer = context.indexBuffer->GetBufferRef();
    cmd.indexCount = context.indexCount;

    PipelineState state = {};
    state.shader = context.material->GetShader();
    state.topology = PrimitiveTopology::TriangleList;

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
