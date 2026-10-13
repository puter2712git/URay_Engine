#include "InputActionSnapshot.h"

#include <algorithm>

namespace URay
{

const InputActionValue* InputActionSnapshot::FindAction(InputActionId id) const
{
    const auto iterator = std::find_if(
        actions.begin(),
        actions.end(),
        [id](const InputActionState& state)
        {
            return state.id == id;
        });

    return iterator != actions.end() ? &iterator->value : nullptr;
}

} // namespace URay
