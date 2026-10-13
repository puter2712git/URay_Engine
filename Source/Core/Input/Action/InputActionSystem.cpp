#include "InputActionSystem.h"

namespace URay
{

void InputActionSystem::Update(const InputSnapshot& input)
{
    if (!actionMap)
    {
        snapshot = {};
        return;
    }

    snapshot = evaluator.Evaluate(input, *actionMap);
}

void InputActionSystem::SetActionMap(const InputActionMap* newActionMap)
{
    if (actionMap == newActionMap)
        return;

    actionMap = newActionMap;
    snapshot = {};
}

} // namespace URay
