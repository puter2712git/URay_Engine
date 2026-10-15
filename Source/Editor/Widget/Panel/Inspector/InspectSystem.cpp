#include "InspectSystem.h"

namespace URay
{

void InspectSystem::SetTarget(InspectTarget newTarget)
{
    InspectTarget previousTarget = target;
    target = std::move(newTarget);

    targetChangedRay.Emit(previousTarget, target);
}

void InspectSystem::ClearTarget()
{
    if (target.IsEmpty())
        return;

    SetTarget({});
}

} // namespace URay
