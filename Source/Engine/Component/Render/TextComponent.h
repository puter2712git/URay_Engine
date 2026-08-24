#pragma once

#include "Engine/Component/Component.h"
#include "Engine/Component/IRenderable.h"

#include <string>

namespace URay
{

class Mesh;
class Material;
class Font;

namespace RHI
{
class TextObject;
}

class TextComponent : public Component, public IRenderable
{
    URAY_CLASS(TextComponent, Component)

public:
    TextComponent();
    ~TextComponent() = default;

public:
    RHI::RenderObject* CreateRenderObject() override;

private:
    RHI::TextObject* renderObject = nullptr;

    Font* font = nullptr;
    std::string text;
};

} // namespace URay
