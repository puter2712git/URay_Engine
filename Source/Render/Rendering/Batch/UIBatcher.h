#pragma once

#include "Render/Vertex.h"

#include <vector>

namespace URay::Render
{

class RenderDevice;
class ResourceManager;

class UIBatcher
{
public:
    UIBatcher();
    ~UIBatcher();

public:
    bool Initialize();
    void Finalize();

    void Reset();

private:
    RenderDevice& device;
    ResourceManager& resourceManager;

    std::vector<Vertex> vertices;
};

} // namespace URay::Render
