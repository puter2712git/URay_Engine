#pragma once

#include <Core/Type/TypeAlias.h>
#include <Core/Input/Action/InputAction.h>

#include <vector>

namespace URay
{

struct InputActionState
{
    InputActionId id = {};
    InputActionValue value = {};
};

class InputActionSnapshot
{
    friend class InputActionEvaluator;

public:
    const InputActionValue* FindAction(InputActionId id) const;

    uint64 GetFrameIndex() const { return frameIndex; }

private:
    uint64 frameIndex = 0;
    Vector<InputActionState> actions;
};

} // namespace URay
