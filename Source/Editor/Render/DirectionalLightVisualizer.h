#pragma once

#include "Editor/Render/EditorComponentVisualizer.h"

#include <unordered_map>

namespace URay
{

namespace Render
{
class BillboardObject;
struct BillboardObjectState;
class MeshObject;
struct MeshObjectState;
}

class DirectionalLightVisualizer final : public EditorComponentVisualizer
{
public:
    void OnAdded(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnRemoved(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnPropertyChanged(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component, const Property& property) override;

private:
    struct DirectionalLightVisual
    {
        Render::MeshObject* arrow = nullptr;
        Render::BillboardObject* billboard = nullptr;
    };

    static Render::MeshObjectState MakeArrowState(EditorVisualContext& context, Unit& unit);
    static Render::BillboardObjectState MakeBillboardState(EditorVisualContext& context, Unit& unit, Component& component);

private:
    std::unordered_map<Component*, DirectionalLightVisual> visuals;
};

} // namespace URay
