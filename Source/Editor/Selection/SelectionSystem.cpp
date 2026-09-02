#include "SelectionSystem.h"

namespace URay
{

SelectionSystem::SelectionSystem() = default;

SelectionSystem::~SelectionSystem() = default;

void SelectionSystem::SelectUnit(Unit* unit)
{
    selectedUnit = unit;
    onSelectRay.Emit(selectedUnit);
}

RayHandle SelectionSystem::RegisterOnSelected(EventRay<Unit*>::Callback callback)
{
    RayHandle handle = onSelectRay.Register(callback);
    return handle;
}

void SelectionSystem::UnregisterOnSelected(RayHandle handle)
{
    onSelectRay.Unregister(handle);
}

} // namespace URay
