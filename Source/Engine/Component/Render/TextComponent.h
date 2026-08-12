#pragma once

#include "RenderComponent.h"

#include <string>

namespace URay
{

class Mesh;
class Material;

class Font;

class TextComponent : public RenderComponent
{
    URAY_CLASS(TextComponent, RenderComponent)

public:
    TextComponent();
    ~TextComponent() = default;

public:
    virtual void SubmitCommand(RHI::DrawCommandBuilder& builder) override;

private:
    Font* font = nullptr;

    std::string text;
};

} // namespace URay
