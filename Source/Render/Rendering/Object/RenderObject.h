#pragma once

#include "Core/Math/Matrix.h"

namespace URay::Render
{

class RenderScene;

class RenderObject
{
public:
    virtual ~RenderObject() = default;

public:
    void SetScene(RenderScene* scene) { this->scene = scene; }

    bool IsDirty() const { return isDirty; }
    void SetDirty(bool dirty) { isDirty = dirty; }

    bool IsEnabled() const { return isEnabled; }
    void SetEnabled(bool enabled) { isEnabled = enabled; }

protected:
    void NotifyUpdated();

protected:
    bool isDirty = false;
    bool isEnabled = true;

private:
    RenderScene* scene = nullptr;
};

} // namespace URay::Render
