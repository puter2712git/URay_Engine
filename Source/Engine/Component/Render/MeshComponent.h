#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include <vulkan/vulkan.h>

namespace URay
{

class Mesh;
class Material;

class MeshComponent : public RenderComponent
{
    URAY_CLASS(MeshComponent, RenderComponent)

public:
    MeshComponent();
    ~MeshComponent() = default;

public:
    virtual void SubmitCommand(RHI::DrawCommandBuilder& builder) override;

    virtual const std::string& GetName() const override { return name; }

    Mesh* GetMesh() const { return mesh; }
    void SetMesh(Mesh* newMesh);

    Material* GetMaterial(size_t index = 0) { return materials.size() > index ? materials[index] : nullptr; }
    const std::vector<Material*>& GetMaterials() const { return materials; }

    void SetMaterial(Material* newMaterial, size_t index = 0);
    void SetMaterials(const std::vector<Material*>& newMaterials) { materials = newMaterials; }

protected:
    std::string name = "Mesh";

private:
    Mesh* mesh = nullptr;
    std::vector<Material*> materials;
};

} // namespace URay
