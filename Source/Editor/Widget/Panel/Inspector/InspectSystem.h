#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Panel/Inspector/InspectTarget.h>

namespace URay
{

class SceneSystem;

class InspectSystem
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

public:
    EventRay<const InspectTarget&, const InspectTarget&> onTargetChanged;
    EventRay<const InspectTarget&> onTargetContentChanged;

private:
    SceneSystem& sceneSystem;

    InspectTarget target = {};
};

} // namespace URay
