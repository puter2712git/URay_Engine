#pragma once

#include "Engine/Pickable.h"
#include "RenderComponent.h"

#include <string>

namespace URay
{

class MeshAsset;
class Material;

class FontAsset;

class TextComponent : public RenderComponent, public IPickable
{
    URAY_CLASS(TextComponent, RenderComponent)

public:
    TextComponent();
    ~TextComponent() = default;

public:
    virtual void SubmitCommand(DrawCommandBuilder& builder) override;

    bool Pick(const Vector3& origin, const Vector3& direction, float& outDist) const override;

private:
    FontAsset* font = nullptr;

    std::string text;
};

} // namespace URay
