#pragma once

namespace URay
{

namespace RHI
{
class RenderObject;
}

class IRenderable
{
public:
    virtual RHI::RenderObject* CreateRenderObject() = 0;
};

} // namespace URay
