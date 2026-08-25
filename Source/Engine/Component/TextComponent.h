#pragma once

#include "Engine/Component/RenderComponent.h"

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

class TextComponent : public RenderComponent
{
    URAY_CLASS(TextComponent, RenderComponent)

public:
    TextComponent();
    ~TextComponent() = default;

public:
    Render::RenderObject* CreateRenderObject() override;

protected:
    void UpdateRenderObject() override;

private:
    Font* font = nullptr;
    std::string text;
};

} // namespace URay
