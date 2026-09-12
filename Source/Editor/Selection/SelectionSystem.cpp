#include "SelectionSystem.h"

namespace URay
{

SelectionSystem::SelectionSystem() = default;

SelectionSystem::~SelectionSystem() = default;

void SelectionSystem::SelectUnit(Unit* unit)
{
    Unit* previousUnit = selectedUnit;
    selectedUnit = unit;
    onSelectionChangedRay.Emit(previousUnit, selectedUnit);
}

} // namespace URay
