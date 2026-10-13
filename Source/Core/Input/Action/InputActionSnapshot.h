#pragma once

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
    std::vector<InputActionState> actions;
};

} // namespace URay
