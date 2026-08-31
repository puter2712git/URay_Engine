#pragma once

#include "DrawCommand.h"
#include "DrawCommandContext.h"

#include "Render/Vertex.h"

#include <memory>
#include <vector>

namespace URay::Render
{

class RenderSystem;
class RenderDevice;
class Renderer;
class GPUResourceManager;
class ShaderManager;
class LineBatcher;
class TextBatcher;

class DrawCommandBuilder
{
public:
    DrawCommandBuilder(RenderSystem& renderSystem);
    ~DrawCommandBuilder();

public:
    bool Initialize();
    void Finalize();

    void Reset();

    void FlushLines();
    void FlushTexts();

    void BuildMesh(const MeshCommandContext& context);
    void BuildLine(const LineCommandContext& context);
    void BuildText(const TextCommandContext& context);

    void BuildFromGizmo(const GizmoCommandContext& context);

    const std::vector<DrawCommand>& GetCommands() const { return drawCmds; }

private:
    RenderDevice& device;
    Renderer& renderer;
    GPUResourceManager& resourceManager;
    ShaderManager& shaderManager;

    std::vector<DrawCommand> drawCmds;

    std::unique_ptr<LineBatcher> lineBatcher = nullptr;
    std::unique_ptr<TextBatcher> textBatcher = nullptr;
};

} // namespace URay::Render
