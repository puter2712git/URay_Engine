#pragma once

#include "Render/Rendering/DrawCommand/DrawCommand.h"
#include "Render/Vertex.h"

#include <memory>
#include <vector>

namespace URay::Render
{

class RenderDevice;
class ResourceManager;
class Buffer;
class Shader;
class UIDrawContext;

class UIBatcher
{
public:
    UIBatcher(RenderDevice& device, ResourceManager& resourceManager);
    ~UIBatcher();

public:
    bool Initialize();
    void Finalize();

    std::vector<DrawCommand> Flush(const UIDrawContext& context);

private:
    RenderDevice& device;
    ResourceManager& resourceManager;

    std::unique_ptr<Buffer> vertexBuffer = nullptr;
    std::unique_ptr<Buffer> indexBuffer = nullptr;

    void* mappedVertexData = nullptr;
    void* mappedIndexData = nullptr;

    Shader* shader = nullptr;
};

} // namespace URay::Render
