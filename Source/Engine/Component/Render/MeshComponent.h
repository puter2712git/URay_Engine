#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include <vulkan/vulkan.h>

namespace URay
{

class Mesh;
class Material;

class MeshComponent : public RenderComponent
{
public:
    MeshComponent();
    ~MeshComponent() = default;

public:
    virtual DrawCommand
    SubmitCommand() override;

    Mesh* GetMesh() const
    {
        return mesh;
    }
    void SetMesh(Mesh* newMesh)
    {
        mesh = newMesh;
    }

    Material* GetMaterial() const
    {
        return material;
    }
    void SetMaterial(Material* newMaterial)
    {
        material = newMaterial;
    }

private:
    Mesh* mesh = nullptr;
    Material* material = nullptr;
};

} // namespace URay
