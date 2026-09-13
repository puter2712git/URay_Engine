#pragma once

#include "Editor/Render/Visualizer/EditorComponentVisualizer.h"

namespace URay
{

namespace Render
{
class BillboardObject;
struct BillboardObjectState;
class MeshObject;
struct MeshObjectState;
class RenderScene;
}

class DirectionalLightVisualizer final : public EditorComponentVisualizer
{
public:
    DirectionalLightVisualizer(EditorVisualContext& context, Unit& unit, Component& component);
    ~DirectionalLightVisualizer() override;

public:
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
    Render::RenderScene& renderScene;
    DirectionalLightVisual visual;
};

} // namespace URay
