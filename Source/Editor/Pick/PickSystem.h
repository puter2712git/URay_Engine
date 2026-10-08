#pragma once

#include <Editor/Pick/PickRegistry.h>
#include <Editor/Gizmo/Strategy/GizmoStrategy.h>

#include <Core/Type/Types.h>

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
class TransformGizmo;
class Engine;
class Scene;
struct Ray;

struct PickResult
{
    bool isHit = false;
    Unit* pickedUnit = nullptr;
    GizmoHandleId gizmoHandle = GizmoHandleId::None;
};

class PickSystem
{
public:
    PickSystem(TransformGizmo& gizmo);
    ~PickSystem();

public:
    bool Initialize();
    void Finalize();

    PickResult Pick(CameraComponent* camera, float screenX, float screenY) const;

    Unit* FindTarget(uint32 objectId) const;

private:
    bool PickGizmo(const Ray& worldRay, GizmoHandleId& outHandle) const;

    void OnRenderObjectAdded(Render::RenderScene* scene, Render::RenderObject* object, Unit* unit, Component* component);
    void OnRenderObjectDestroyed(Render::RenderScene* scene, Render::RenderObject* object);
    void OnRenderObjectUpdated(Render::RenderScene* scene, Render::RenderObject* object);

private:
    TransformGizmo& gizmo;

    PickRegistry pickRegistry;
    std::unordered_map<Render::RenderObject*, std::unique_ptr<PickObject>> pickObjects;

    std::unordered_map<uint32, Unit*> objectTargets;
};

} // namespace URay
