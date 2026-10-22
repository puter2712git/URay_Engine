#include "EditorInputActions.h"

#include <Core/Input/Action/InputActionMap.h>

namespace URay
{

std::unique_ptr<InputActionMap> CreateEditorInputActionMap()
{
    auto actionMap = std::make_unique<InputActionMap>();

    std::vector<InputBinding> viewportMoveBindings;
    viewportMoveBindings.push_back(Axis2DCompositeBinding{
        .negativeX = KeyCode::A,
        .positiveX = KeyCode::D,
        .negativeY = KeyCode::S,
        .positiveY = KeyCode::W });

    const bool addedViewportMove = actionMap->AddAction(
        { .id = EditorInputAction::ViewportMove,
          .valueType = InputActionValueType::Axis2D,
          .bindings = viewportMoveBindings });

    std::vector<InputBinding> viewportElevateBindings;
    viewportElevateBindings.push_back(Axis1DCompositeBinding{
        .negative = KeyCode::Q,
        .positive = KeyCode::E });

    const bool addedViewportElevate = actionMap->AddAction(
        { .id = EditorInputAction::ViewportElevate,
          .valueType = InputActionValueType::Axis1D,
          .bindings = viewportElevateBindings });

    const bool addedSaveScene = actionMap->AddAction(
        { .id = EditorInputAction::SaveScene,
          .valueType = InputActionValueType::Button,
          .bindings = { KeyBinding{
              .key = KeyCode::S,
              .requiredModifiers = ModifierKey::Ctrl } } });

    const bool addedDeleteSelection = actionMap->AddAction(
        { .id = EditorInputAction::DeleteSelection,
          .valueType = InputActionValueType::Button,
          .bindings = { KeyBinding{ .key = KeyCode::Delete } } });

    const bool addedCycleGizmoMode = actionMap->AddAction(
        { .id = EditorInputAction::CycleGizmoMode,
          .valueType = InputActionValueType::Button,
          .bindings = { KeyBinding{ .key = KeyCode::Space } } });

    const bool addedToggleEditorCamera = actionMap->AddAction(
        { .id = EditorInputAction::ToggleEditorCamera,
          .valueType = InputActionValueType::Button,
          .bindings = { KeyBinding{ .key = KeyCode::GraveAccent } } });

    const bool addedFocusActiveUnit = actionMap->AddAction(
        { .id = EditorInputAction::FocusActiveUnit,
          .valueType = InputActionValueType::Button,
          .bindings = { KeyBinding{ .key = KeyCode::F } } });

    if (!addedViewportMove ||
        !addedViewportElevate ||
        !addedSaveScene ||
        !addedDeleteSelection ||
        !addedCycleGizmoMode ||
        !addedToggleEditorCamera ||
        !addedFocusActiveUnit)
        return nullptr;

    return actionMap;
}

} // namespace URay
