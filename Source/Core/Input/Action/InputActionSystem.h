#pragma once

#include <Core/Input/Action/InputActionEvaluator.h>
#include <Core/Input/Action/InputActionSnapshot.h>

namespace URay
{

class InputActionMap;
struct InputSnapshot;

class InputActionSystem
{
public:
    void Update(const InputSnapshot& input);

    const InputActionSnapshot& GetSnapshot() const { return snapshot; }
    void SetActionMap(const InputActionMap* newActionMap);

private:
    const InputActionMap* actionMap = nullptr;

    InputActionEvaluator evaluator;
    InputActionSnapshot snapshot;
};

} // namespace URay
