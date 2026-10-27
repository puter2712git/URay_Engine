#pragma once

#include <Engine/Asset/AssetHandle.h>
#include <Engine/Component/Render/RenderComponent.h>

namespace URay
{

class MeshComponent : public RenderComponent
{
    URAY_OBJECT(MeshComponent, RenderComponent)

public:
    MeshComponent();

public:
    Render::RenderObject* CreateRenderObject() override;

    const AssetHandle& GetMeshHandle() const { return meshHandle; }
    void SetMeshHandle(const AssetHandle& newMeshHandle);

    const AssetHandle& GetMaterialHandle(size_t index = 0) const { return materialHandles.size() > index ? materialHandles[index] : AssetHandle{}; }
    const std::vector<AssetHandle>& GetMaterials() const { return materialHandles; }

    void SetMaterial(const AssetHandle& newMaterialHandle, size_t index = 0);
    void SetMaterials(const std::vector<AssetHandle>& newMaterialHandles);

protected:
    void UpdateRenderObject() override;

private:
    AssetHandle meshHandle = {};
    std::vector<AssetHandle> materialHandles;

    bool castsShadow = true;
};

} // namespace URay
