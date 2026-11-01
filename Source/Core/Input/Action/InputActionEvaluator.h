#pragma once

#include <Core/Input/Action/InputActionMap.h>
#include <Core/Input/Action/InputActionSnapshot.h>
#include <Core/Type/TypeAlias.h>

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
