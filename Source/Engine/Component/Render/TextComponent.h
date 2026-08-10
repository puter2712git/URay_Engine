#pragma once

#include "RenderComponent.h"

#include <string>

namespace URay
{

class MeshAsset;
class Material;

class FontAsset;

class TextComponent : public RenderComponent
{
    URAY_CLASS(TextComponent, RenderComponent)

public:
    TextComponent();
    ~TextComponent() = default;

public:
    virtual void SubmitCommand(DrawCommandBuilder& builder) override;

private:
    FontAsset* font = nullptr;

    std::string text;
};

} // namespace URay
