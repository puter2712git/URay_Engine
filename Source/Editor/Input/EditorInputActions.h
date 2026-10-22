#pragma once

#include <Core/Input/Action/InputAction.h>

#include <memory>

namespace URay
{

class InputActionMap;

std::unique_ptr<InputActionMap> CreateEditorInputActionMap();

} // namespace URay

namespace URay::EditorInputAction
{

inline constexpr InputActionId ViewportMove = { 1 };
inline constexpr InputActionId ViewportElevate = { 2 };

inline constexpr InputActionId ReloadGameScripts = { 3 };
inline constexpr InputActionId SaveScene = { 4 };
inline constexpr InputActionId DeleteSelection = { 5 };
inline constexpr InputActionId CycleGizmoMode = { 6 };
inline constexpr InputActionId ToggleEditorCamera = { 7 };
inline constexpr InputActionId FocusActiveUnit = { 8 };

} // namespace URay::EditorInputAction
