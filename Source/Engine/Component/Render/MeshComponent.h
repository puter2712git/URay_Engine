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
    virtual void SubmitCommand(DrawCommandBuilder& builder) override;

    virtual void GetProperties(std::vector<Property>& properties) override;

    virtual const std::string& GetName() const override { return name; }

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

protected:
    std::string name = "Mesh";

private:
    Mesh* mesh = nullptr;
    Material* material = nullptr;
};

} // namespace URay
