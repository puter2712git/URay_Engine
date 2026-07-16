#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include <vulkan/vulkan.h>

namespace URay
{

class Mesh;

class MeshComponent : public RenderComponent
{
public:
    MeshComponent();
    ~MeshComponent() = default;

public:
    virtual DrawCommand SubmitCommand() override;

    Mesh* GetMesh() const
    {
        return mesh;
    }
    void SetMesh(Mesh* newMesh)
    {
        mesh = newMesh;
    }

private:
    Mesh* mesh = nullptr;
};

} // namespace URay
