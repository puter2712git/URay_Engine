#pragma once

#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/AssetHandle.h>
#include <Engine/Component/Render/RenderComponent.h>

#include <vector>

namespace URay
{

class SkinnedMeshComponent : public RenderComponent
{
    URAY_OBJECT(SkinnedMeshComponent, RenderComponent)

public:
    SkinnedMeshComponent();

public:
    Render::RenderObject* CreateRenderObject() override;

    const AssetHandle& GetSkinnedMeshHandle() const { return skinnedMeshHandle; }
    void SetSkinnedMeshHandle(const AssetHandle& handle);

    const AssetHandle& GetMaterialHandle(size_t index = 0) const { return materialHandles.size() > index ? materialHandles[index] : AssetHandle{}; }
    const Vector<AssetHandle>& GetMaterials() const { return materialHandles; }

    void SetMaterial(const AssetHandle& newMaterialHandle, size_t index = 0);
    void SetMaterials(const Vector<AssetHandle>& newMaterialHandles);

protected:
    void UpdateRenderObject() override;

private:
    AssetHandle skinnedMeshHandle = {};
    Vector<AssetHandle> materialHandles;

    bool castsShadow = true;
};

} // namespace URay
