#pragma once

#include <Core/Type/Types.h>
#include <Core/Type/UUID.h>
#include <Engine/Scene/Scene.h>
#include <Engine/UI/Widget/Widget.h>

#include <string_view>

namespace URay
{

class Unit;
class HorizontalLayout;
class Label;
class Surface;
class SelectionSystem;

class SceneTreeNode final : public Widget
{
public:
    SceneTreeNode();

public:
    void Bind(SceneId sceneId, const UUID& unitId, std::string_view name, uint32 depth);
    void Unbind();

    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnUpdate(float deltaTime) override;

private:
    Unit* ResolveUnit() const;

private:
    SelectionSystem* selectionSystem = nullptr;

    SceneId sceneId = InvalidSceneId;
    UUID unitId = {};
    uint32 depth = 0;

    Surface* surface = nullptr;
    HorizontalLayout* layout = nullptr;
    Label* label = nullptr;
};

} // namespace URay
