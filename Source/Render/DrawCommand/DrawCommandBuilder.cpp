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

} // namespace URay
