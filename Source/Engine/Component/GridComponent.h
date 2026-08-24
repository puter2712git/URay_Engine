#pragma once

#include "Engine/Component/Component.h"
#include "Engine/Component/IRenderable.h"

namespace URay
{

namespace Render
{
class LineObject;
}

class GridComponent : public Component, public IRenderable
{
    URAY_CLASS(GridComponent, Component)

public:
    Render::RenderObject* CreateRenderObject() override;

private:
    Render::LineObject* renderObject = nullptr;
};

} // namespace URay
