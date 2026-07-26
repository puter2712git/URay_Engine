#pragma once

#include "Engine/Component/Render/RenderComponent.h"

namespace URay
{

class TextureAsset;
class Mesh;
class Material;

class SpriteComponent : public RenderComponent
{
    URAY_CLASS(SpriteComponent, RenderComponent)

public:
    SpriteComponent();
    virtual ~SpriteComponent() override = default;

public:
    void SubmitCommand(DrawCommandBuilder& builder) override;

private:
    TextureAsset* texture = nullptr;

    Mesh* quadMesh = nullptr;
    Material* material = nullptr;
};

} // namespace URay
