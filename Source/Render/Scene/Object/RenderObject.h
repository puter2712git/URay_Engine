#pragma once

#include "Core/Math/Matrix.h"

namespace URay::Render
{

class RenderObject
{
public:
    virtual ~RenderObject() = default;

public:
    bool IsDirty() const { return isDirty; }
    void SetDirty(bool dirty) { isDirty = dirty; }

protected:
    Matrix worldMatrix = Matrix::Identity;

    bool isDirty = false;
};

} // namespace URay::Render
