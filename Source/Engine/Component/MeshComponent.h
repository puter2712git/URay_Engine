#pragma once

#include "Engine/Component/RenderComponent.h"

#include "Core/Math/AABB.h"

#include <vulkan/vulkan.h>

namespace URay
{

class Mesh;
class Material;

namespace Render
{
class MeshObject;
}

class MeshComponent : public RenderComponent
{
    URAY_CLASS(MeshComponent, RenderComponent)

public:
    MeshComponent();
    ~MeshComponent() = default;

public:
    Render::RenderObject* CreateRenderObject() override;

    Mesh* GetMesh() const { return mesh; }
    void SetMesh(Mesh* newMesh);

    Material* GetMaterial(size_t index = 0) { return materials.size() > index ? materials[index] : nullptr; }
    const std::vector<Material*>& GetMaterials() const { return materials; }

    void SetMaterial(Material* newMaterial, size_t index = 0);
    void SetMaterials(const std::vector<Material*>& newMaterials) { materials = newMaterials; }

protected:
    void UpdateRenderObject() override;

private:
    Mesh* mesh = nullptr;
    std::vector<Material*> materials;
};

} // namespace URay
