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

public:
    EventRay<const InspectTarget&, const InspectTarget&> onTargetChanged;

private:
    InspectTarget target = {};
};

} // namespace URay
