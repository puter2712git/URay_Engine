#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include "Core/Math/AABB.h"

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
    virtual void OnAttached() override;
    virtual void OnDetached() override;

    virtual void SubmitCommand(RHI::DrawCommandBuilder& builder) override;

    Mesh* GetMesh() const { return mesh; }
    void SetMesh(Mesh* newMesh);

    Material* GetMaterial(size_t index = 0) { return materials.size() > index ? materials[index] : nullptr; }
    const std::vector<Material*>& GetMaterials() const { return materials; }

    void SetMaterial(Material* newMaterial, size_t index = 0);
    void SetMaterials(const std::vector<Material*>& newMaterials) { materials = newMaterials; }

    const AABB& GetWorldBounds() const { return worldBounds; }

private:
    void UpdateWorldBounds();

private:
    Mesh* mesh = nullptr;
    std::vector<Material*> materials;

    AABB worldBounds = {};
};

} // namespace URay
