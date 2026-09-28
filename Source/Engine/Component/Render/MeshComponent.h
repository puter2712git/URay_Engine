#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include "Core/Math/AABB.h"
#include "Core/UUID.h"

#include <vulkan/vulkan.h>

namespace URay
{

class Mesh;
class Material;

namespace Render
{
}

class MeshComponent : public RenderComponent
{
    URAY_CLASS(MeshComponent, RenderComponent)

public:
    MeshComponent();
    ~MeshComponent() = default;

public:
    Render::RenderObject* CreateRenderObject() override;

    const UUID& GetMeshUUID() const { return meshUUID; }
    void SetMeshUUID(const UUID& newMeshUUID);

    const UUID& GetMaterialUUID(size_t index = 0) const { return materialUUIDs.size() > index ? materialUUIDs[index] : UUID{}; }
    const std::vector<UUID>& GetMaterials() const { return materialUUIDs; }

    void SetMaterial(const UUID& newMaterialUUID, size_t index = 0);
    void SetMaterials(const std::vector<UUID>& newMaterialUUIDs) { materialUUIDs = newMaterialUUIDs; }

protected:
    void UpdateRenderObject() override;

private:
    UUID meshUUID = {};
    std::vector<UUID> materialUUIDs;

    bool castsShadow = true;
};

} // namespace URay
