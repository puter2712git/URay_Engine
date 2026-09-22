#pragma once

#include "Render/Rendering/Object/RenderObject.h"

namespace URay::Render
{

class DrawCommandBuilder;

class DrawableObject : public RenderObject
{
public:
    ~DrawableObject() override = default;

public:
    virtual void Submit(DrawCommandBuilder& builder) const = 0;
    virtual void SubmitSelectionMask(DrawCommandBuilder& builder) const {}
};

} // namespace URay::Render
