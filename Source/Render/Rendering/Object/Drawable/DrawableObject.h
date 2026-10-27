#pragma once

#include <Render/Rendering/Object/RenderObject.h>

namespace URay::Render
{

class DrawCommandBuilder;
struct DrawCommandParams;

class DrawableObject : public RenderObject
{
    URAY_CLASS(DrawableObject, RenderObject)

public:
    ~DrawableObject() override = default;

public:
    virtual void Submit(DrawCommandBuilder& builder, const DrawCommandParams& params) const = 0;
    virtual void SubmitSelectionMask(DrawCommandBuilder& builder, const DrawCommandParams& params) const {}
};

} // namespace URay::Render
