#pragma once

#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Pickable.h"

namespace URay
{

class TextureAsset;
class MeshAsset;
class Material;

class SpriteComponent : public RenderComponent, public IPickable
{
    URAY_CLASS(SpriteComponent, RenderComponent)

public:
    SpriteComponent();
    virtual ~SpriteComponent() override = default;

public:
    void SubmitCommand(DrawCommandBuilder& builder) override;

    bool Pick(const Vector3& origin, const Vector3& direction, float& outDist) const override;

    Material* GetMaterial() const { return material; }

private:
    TextureAsset* texture = nullptr;

    MeshAsset* quadMesh = nullptr;
    Material* material = nullptr;
};

} // namespace URay
