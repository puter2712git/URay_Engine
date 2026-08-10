#pragma once

#include "Engine/Component/Render/RenderComponent.h"

namespace URay
{

class TextureAsset;
class MeshAsset;
class Material;

class SpriteComponent : public RenderComponent
{
    URAY_CLASS(SpriteComponent, RenderComponent)

public:
    SpriteComponent();
    virtual ~SpriteComponent() override = default;

public:
    void SubmitCommand(DrawCommandBuilder& builder) override;

    Material* GetMaterial() const { return material; }

private:
    TextureAsset* texture = nullptr;

    MeshAsset* quadMesh = nullptr;
    Material* material = nullptr;
};

} // namespace URay
