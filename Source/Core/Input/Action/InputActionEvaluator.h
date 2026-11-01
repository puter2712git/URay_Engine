#pragma once

#include <Core/Type/TypeAlias.h>
#include <Core/Input/Action/InputActionMap.h>
#include <Core/Input/Action/InputActionSnapshot.h>

#include <vector>

namespace URay
{

struct InputSnapshot;

class InputActionEvaluator
{
public:
    InputActionSnapshot Evaluate(const InputSnapshot& input, const InputActionMap& actionMap);

private:
    Vector<InputActionState> previousActions;
};

} // namespace URay
