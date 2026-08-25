#pragma once

#include "Engine/Component/RenderComponent.h"

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
    virtual ~SpriteComponent() override = default;

public:
    Render::RenderObject* CreateRenderObject() override;

    Material* GetMaterial() const { return material; }

protected:
    void UpdateRenderObject() override;

private:
    Texture* texture = nullptr;

    Mesh* quadMesh = nullptr;
    Material* material = nullptr;
};

} // namespace URay
