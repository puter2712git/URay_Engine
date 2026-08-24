#pragma once

namespace URay
{

namespace Render
{
class RenderObject;
}

class IRenderable
{
public:
    virtual Render::RenderObject* CreateRenderObject() = 0;
};

} // namespace URay
