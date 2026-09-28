#pragma once

#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Asset/Asset.h"

#include <string>

namespace URay
{

class Font;

namespace Render
{
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
    AssetHandle fontHandle = {};
    std::string text;
};

} // namespace URay
