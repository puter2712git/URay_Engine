#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Panel/Inspector/InspectTarget.h>

namespace URay
{

class InspectSystem
{
public:
    void SetTarget(InspectTarget target);
    void ClearTarget();

    const InspectTarget& GetTarget() const { return target; }

    EventRay<const InspectTarget&, const InspectTarget&>& GetOnTargetChangedRay() { return onTargetChanged; }

private:
    InspectTarget target = {};

    EventRay<const InspectTarget&, const InspectTarget&> onTargetChanged;
};

} // namespace URay
