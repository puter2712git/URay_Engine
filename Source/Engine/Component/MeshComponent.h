#pragma once

#include "Engine/Component/Component.h"
#include "Engine/Component/IRenderable.h"

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

class MeshComponent : public Component, public IRenderable
{
    URAY_CLASS(MeshComponent, Component)

public:
    MeshComponent();
    ~MeshComponent() = default;

public:
    void Update(float deltaTime) override;
    void OnAttached() override;
    void OnDetached() override;

    Render::RenderObject* CreateRenderObject() override;

    Mesh* GetMesh() const { return mesh; }
    void SetMesh(Mesh* newMesh);

    Material* GetMaterial(size_t index = 0) { return materials.size() > index ? materials[index] : nullptr; }
    const std::vector<Material*>& GetMaterials() const { return materials; }

    void SetMaterial(Material* newMaterial, size_t index = 0);
    void SetMaterials(const std::vector<Material*>& newMaterials) { materials = newMaterials; }

private:
    Render::MeshObject* renderObject = nullptr;

    Mesh* mesh = nullptr;
    std::vector<Material*> materials;
};

} // namespace URay
