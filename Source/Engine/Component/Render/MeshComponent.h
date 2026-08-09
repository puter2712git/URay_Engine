#pragma once

#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Pickable.h"

#include <vulkan/vulkan.h>

namespace URay
{

class MeshAsset;
class Material;

class MeshComponent : public RenderComponent, public IPickable
{
    URAY_CLASS(MeshComponent, RenderComponent)

public:
    MeshComponent();
    ~MeshComponent() = default;

public:
    virtual void SubmitCommand(DrawCommandBuilder& builder) override;

    bool Pick(const Vector3& origin, const Vector3& direction, float& outDist) const override;

    virtual const std::string& GetName() const override { return name; }

    MeshAsset* GetMesh() const { return mesh; }
    void SetMesh(MeshAsset* newMesh) { mesh = newMesh; }

    Material* GetMaterial(size_t index = 0) { return materials.size() > index ? materials[index] : nullptr; }
    const std::vector<Material*>& GetMaterials() const { return materials; }

    void SetMaterial(Material* newMaterial, size_t index = 0)
    {
        if (materials.size() <= index)
        {
            materials[index] = newMaterial;
        }
    }
    void SetMaterials(const std::vector<Material*>& newMaterials) { materials = newMaterials; }

protected:
    std::string name = "Mesh";

private:
    MeshAsset* mesh = nullptr;
    std::vector<Material*> materials;
};

} // namespace URay
