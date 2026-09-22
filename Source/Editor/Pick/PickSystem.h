#pragma once

#include "Editor/Pick/PickRegistry.h"

#include "Core/Type/Types.h"

#include <memory>
#include <unordered_map>

namespace URay::Render
{
class RenderObject;
class RenderScene;
} // namespace URay::Render

namespace URay
{

class Component;
class CameraComponent;
class PickObject;
class Unit;
class GizmoController;
class Engine;
class Scene;
struct Ray;

struct PickResult
{
    bool isHit = false;
    Unit* pickedUnit = nullptr;
    int32 gizmoAxis = -1;
};

class PickSystem
{
public:
    PickSystem(GizmoController& gizmo);
    ~PickSystem();

public:
    bool Initialize();
    void Finalize();

    PickResult Pick(CameraComponent* camera, float screenX, float screenY) const;

private:
    bool PickGizmo(const Ray& worldRay, int32& outAxis) const;

    void OnRenderObjectAdded(Render::RenderScene* scene, Render::RenderObject* object, Unit* unit, Component* component);
    void OnRenderObjectDestroyed(Render::RenderScene* scene, Render::RenderObject* object);
    void OnRenderObjectUpdated(Render::RenderScene* scene, Render::RenderObject* object);

private:
    GizmoController& gizmo;

    PickRegistry pickRegistry;
    std::unordered_map<Render::RenderObject*, std::unique_ptr<PickObject>> pickObjects;
};

} // namespace URay
