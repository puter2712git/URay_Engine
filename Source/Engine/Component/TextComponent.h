#pragma once

#include "Engine/Component/Component.h"
#include "Engine/Component/IRenderable.h"

#include <string>

namespace URay
{

class Mesh;
class Material;
class Font;

namespace Render
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
    Render::RenderObject* CreateRenderObject() override;

private:
    Render::TextObject* renderObject = nullptr;

    Font* font = nullptr;
    std::string text;
};

} // namespace URay
