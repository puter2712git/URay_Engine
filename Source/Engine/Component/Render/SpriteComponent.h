#pragma once

#include "Engine/Component/Component.h"
#include "Engine/Component/IRenderable.h"

namespace URay
{

class Texture;
class Mesh;
class Material;

namespace RHI
{
class MeshObject;
}

class SpriteComponent : public Component, public IRenderable
{
    URAY_CLASS(SpriteComponent, Component)

public:
    SpriteComponent();
    virtual ~SpriteComponent() override = default;

public:
    RHI::RenderObject* CreateRenderObject() override;

    Material* GetMaterial() const { return material; }

private:
    RHI::MeshObject* renderObject = nullptr;

    Texture* texture = nullptr;

    Mesh* quadMesh = nullptr;
    Material* material = nullptr;
};

} // namespace URay
