#pragma once

#include <Core/Type/Ray/RayEmitter.h>
#include <Editor/Widget/Panel/Inspector/InspectTarget.h>

namespace URay
{

class SceneSystem;
class Scene;
class Component;
struct Property;

class InspectSystem : public RayEmitter
{
public:
    InspectSystem(SceneSystem& sceneSystem);

public:
    bool Initialize();
    void Finalize();

    void SetTarget(InspectTarget target);
    void ClearTarget();

    const InspectTarget& GetTarget() const { return target; }

private:
    Unit* GetInspectedUnit() const;

    bool IsInspecting(const Unit* unit) const;

    void OnComponentChanged(Scene* scene, Unit* unit, Component* component);
    void OnComponentPropertyChanged(Scene* scene, Unit* unit, Component* component, const Property* property);
    void OnUnitRemoved(Scene* scene, Unit* unit);
    void OnUnitWorldTransformUpdated(Scene* scene, Unit* unit);

private:
    SceneSystem& sceneSystem;

    InspectTarget target = {};
};

} // namespace URay
