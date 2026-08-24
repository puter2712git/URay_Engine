#pragma once

#include "Engine/Component/Component.h"
#include "Engine/Component/IRenderable.h"

namespace URay
{

namespace RHI
{
class LineObject;
}

class GridComponent : public Component, public IRenderable
{
    URAY_CLASS(GridComponent, Component)

public:
    RHI::RenderObject* CreateRenderObject() override;

private:
    RHI::LineObject* renderObject = nullptr;
};

} // namespace URay
