#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Type/RuntimeType.h"
#include "Core/Type/Types.h"

namespace URay::Render
{

class RenderScene;

class RenderObject
{
    URAY_ROOT_TYPE(RenderObject)

public:
    virtual ~RenderObject() = default;

public:
    void SetScene(RenderScene* scene) { this->scene = scene; }

    bool IsDirty() const { return isDirty; }
    void SetDirty(bool dirty) { isDirty = dirty; }

    bool IsEnabled() const { return isEnabled; }
    void SetEnabled(bool enabled) { isEnabled = enabled; }

    uint32 GetId() const { return id; }
    void SetId(uint32 id) { this->id = id; }

protected:
    void NotifyUpdated();

protected:
    bool isDirty = false;
    bool isEnabled = true;

private:
    RenderScene* scene = nullptr;

    uint32 id = 0;
};

} // namespace URay::Render
