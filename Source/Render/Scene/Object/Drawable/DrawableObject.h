#pragma once

#include "Render/Scene/Object/RenderObject.h"

namespace URay::Render
{

class DrawCommandBuilder;

class DrawableObject : public RenderObject
{
public:
    ~DrawableObject() override = default;

public:
    virtual void Submit(DrawCommandBuilder& builder) const = 0;
};

} // namespace URay::Render
