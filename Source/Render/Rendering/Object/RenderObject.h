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

    bool IsEnabled() const { return isEnabled; }
    void SetEnabled(bool enabled) { isEnabled = enabled; }

protected:
    bool isDirty = false;
    bool isEnabled = true;
};

} // namespace URay::Render
