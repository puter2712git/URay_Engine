#pragma once

#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Pickable.h"

#include <vulkan/vulkan.h>

namespace URay
{

class Mesh;
class Material;

class MeshComponent : public RenderComponent, public IPickable
{
    URAY_CLASS(MeshComponent, RenderComponent)

public:
    MeshComponent();
    ~MeshComponent() = default;

public:
    virtual void SubmitCommand(DrawCommandBuilder& builder) override;

    virtual void GetProperties(std::vector<Property>& properties) override;

    bool Pick(const Vector3& origin, const Vector3& direction, float& outDist) const override;

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
