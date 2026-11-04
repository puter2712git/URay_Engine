#pragma once

#include <Engine/Asset/AssetHandle.h>
#include <Engine/Component/Render/RenderComponent.h>

namespace URay
{

class MaterialInstance;

class SpriteComponent : public RenderComponent
{
    URAY_OBJECT(SpriteComponent, RenderComponent)

public:
    SpriteComponent();
    ~SpriteComponent() override = default;

public:
    Render::RenderObject* CreateRenderObject() override;

    const AssetHandle& GetTextureHandle() const { return textureHandle; }
    const AssetHandle& GetQuadMeshHandle() const { return quadMeshHandle; }
    const AssetHandle& GetMaterialHandle() const { return materialHandle; }

protected:
    void UpdateRenderObject() override;

private:
    MaterialInstance* GetOrCreateMaterialInstance();

private:
    AssetHandle textureHandle = {};

    AssetHandle quadMeshHandle = {};
    AssetHandle materialHandle = {};
    AssetHandle materialInstanceHandle = {};
};

} // namespace URay
