#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include "Core/UUID.h"

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

    const UUID& GetTextureUUID() const { return textureUUID; }
    const UUID& GetQuadMeshUUID() const { return quadMeshUUID; }
    const UUID& GetMaterialUUID() const { return materialUUID; }

protected:
    void UpdateRenderObject() override;

private:
    UUID textureUUID = {};

    UUID quadMeshUUID = {};
    UUID materialUUID = {};
};

} // namespace URay
