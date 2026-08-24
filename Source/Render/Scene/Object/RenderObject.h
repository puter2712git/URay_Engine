#pragma once

#include "Core/Math/Matrix.h"

namespace URay::Render
{

class DrawCommandBuilder;

class RenderObject
{
public:
    virtual ~RenderObject() = default;

public:
    virtual void Submit(DrawCommandBuilder& builder) const {}

    bool IsDirty() const { return isDirty; }
    void SetDirty(bool dirty) { isDirty = dirty; }

protected:
    Matrix worldMatrix = Matrix::Identity;

    bool isDirty = false;
};

} // namespace URay::Render
