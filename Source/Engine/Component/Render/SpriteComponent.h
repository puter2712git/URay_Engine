#pragma once

#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Asset/Asset.h"

namespace URay
{

class Texture;
class Mesh;
class Material;

class SpriteComponent : public RenderComponent
{
    URAY_CLASS(SpriteComponent, RenderComponent)

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
    AssetHandle textureHandle = {};

    AssetHandle quadMeshHandle = {};
    AssetHandle materialHandle = {};
};

} // namespace URay
