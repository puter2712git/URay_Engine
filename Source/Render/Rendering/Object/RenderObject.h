#pragma once

#include <Core/Math/AABB.h>
#include <Core/Math/Matrix.h>
#include <Core/Type/Type.h>
#include <Core/Type/TypeAlias.h>

namespace URay::Render
{

class RenderScene;

class RenderObject
{
    URAY_TYPE(RenderObject)

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

    virtual const AABB* TryGetWorldBounds() const { return nullptr; }

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
