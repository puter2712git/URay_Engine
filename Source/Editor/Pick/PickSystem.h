#pragma once

#include "Core/Type/Types.h"
#include "Editor/Pick/PickRegistry.h"

#include <memory>
#include <unordered_map>

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
    PickSystem(Engine& engine, GizmoController& gizmo);
    ~PickSystem();

public:
    bool Initialize();
    void Finalize();

    PickResult Pick(CameraComponent* camera, float screenX, float screenY) const;

private:
    bool PickGizmo(const Ray& worldRay, int32& outAxis) const;

    void OnUnitAdded(Scene* scene, Unit* unit);
    void OnUnitRemoved(Scene* scene, Unit* unit);
    void OnComponentPropertyChanged(Scene* scene, Unit* unit, Component* component, const Property& property);

private:
    Engine& engine;
    GizmoController& gizmo;

    PickRegistry pickRegistry;
    std::unordered_map<Component*, std::unique_ptr<PickObject>> pickObjects;
};

} // namespace URay
