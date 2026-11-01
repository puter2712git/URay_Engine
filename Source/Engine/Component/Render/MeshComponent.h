#pragma once

#include <Core/Type/TypeAlias.h>
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
    const Vector<AssetHandle>& GetMaterials() const { return materialHandles; }

    void SetMaterial(const AssetHandle& newMaterialHandle, size_t index = 0);
    void SetMaterials(const Vector<AssetHandle>& newMaterialHandles);

protected:
    void UpdateRenderObject() override;

private:
    AssetHandle meshHandle = {};
    Vector<AssetHandle> materialHandles;

    bool castsShadow = true;
};

} // namespace URay
