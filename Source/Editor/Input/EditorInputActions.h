#pragma once

#include <Core/Input/Action/InputAction.h>

#include <memory>

namespace URay
{

class InputActionMap;

namespace EditorInputAction
{

inline constexpr InputActionId ViewportMove = { 1 };
inline constexpr InputActionId ViewportElevate = { 2 };

} // namespace EditorInputAction

std::unique_ptr<InputActionMap> CreateEditorInputActionMap();

} // namespace URay
