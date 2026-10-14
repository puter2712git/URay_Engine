#include "CameraVisualizer.h"

#include <Editor/Selection/SelectionSystem.h>

#include <Core/Math/Math.h>
#include <Core/Math/Vector4.h>

#include <Engine/Component/Render/CameraComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Engine.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/Unit.h>

#include <Render/RenderSystem.h>
#include <Render/Rendering/Object/Drawable/LineObject.h>
#include <Render/Rendering/Scene/RenderScene.h>
#include <Render/Rendering/Scene/SceneSystem.h>

#include <cmath>
#include <memory>

namespace URay
{

CameraVisualizer::CameraVisualizer(EditorVisualContext& context, Unit& unit, Component& component, SelectionSystem& selectionSystem)
    : engine(context.engine), renderScene(context.renderScene), selectionSystem(selectionSystem)
{
    selectionSystem.GetOnActiveSelectionChangedRay().Register(
        this,
        [this](Unit* previousUnit, Unit* selectedUnit)
        { OnSelectionChanged(previousUnit, selectedUnit); });

    visual = { .unit = &unit, .component = &component };
    if (selectionSystem.GetActiveUnit() == &unit)
        CreateLine(context, component, visual);
}

CameraVisualizer::~CameraVisualizer()
{
    selectionSystem.GetOnActiveSelectionChangedRay().UnregisterAll(this);

    EditorVisualContext context = {
        .engine = engine,
        .renderScene = renderScene
    };
    DestroyLine(context, visual);
}

Render::LineObjectState CameraVisualizer::MakeLineState(Component& component)
{
    const CameraComponent& camera = static_cast<const CameraComponent&>(component);
    Vector3 corners[8];

    const Extent2D& viewportExtent = camera.GetViewportExtent();
    if (viewportExtent.width > 0 && viewportExtent.height > 0)
    {
        const Matrix inverseProjection = camera.GetProjMatrix().Inverse();
        const Matrix inverseView = camera.GetViewMatrix().Inverse();

        const Vector4 ndcCorners[8] = {
            { -1.0f, -1.0f, 0.0f, 1.0f },
            { 1.0f, -1.0f, 0.0f, 1.0f },
            { 1.0f, 1.0f, 0.0f, 1.0f },
            { -1.0f, 1.0f, 0.0f, 1.0f },
            { -1.0f, -1.0f, 1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f, 1.0f },
            { -1.0f, 1.0f, 1.0f, 1.0f }
        };

        for (uint32 i = 0; i < 8; ++i)
        {
            const Vector4 view = ndcCorners[i] * inverseProjection;
            const Vector4 world = Vector4(view.x / view.w, view.y / view.w, view.z / view.w, 1.0f) * inverseView;
            corners[i] = Vector3(world.x / world.w, world.y / world.w, world.z / world.w);
        }
    }
    else
    {
        const Unit* unit = component.GetOwner();
        const TransformComponent* transform = unit ? unit->GetTransform() : nullptr;
        const Vector3 origin = transform ? transform->GetWorldPosition() : Vector3::Zero;
        const Vector3 forward = transform ? transform->GetWorldForward() : Vector3::Forward;
        const Vector3 right = transform ? transform->GetWorldRight() : Vector3::Right;
        const Vector3 up = transform ? transform->GetWorldUp() : Vector3::Up;

        const float nearDistance = camera.GetNearPlane();
        const float farDistance = camera.GetFarPlane();
        const float nearHalfHeight = camera.IsPerspective()
                                         ? nearDistance * std::tan(Math::DegToRad(camera.GetFOV() * 0.5f))
                                         : camera.GetOrthoSize() * 0.5f;
        const float farHalfHeight = camera.IsPerspective()
                                        ? farDistance * std::tan(Math::DegToRad(camera.GetFOV() * 0.5f))
                                        : camera.GetOrthoSize() * 0.5f;

        const Vector3 nearCenter = origin + forward * nearDistance;
        const Vector3 farCenter = origin + forward * farDistance;
        corners[0] = nearCenter - right * nearHalfHeight - up * nearHalfHeight;
        corners[1] = nearCenter + right * nearHalfHeight - up * nearHalfHeight;
        corners[2] = nearCenter + right * nearHalfHeight + up * nearHalfHeight;
        corners[3] = nearCenter - right * nearHalfHeight + up * nearHalfHeight;
        corners[4] = farCenter - right * farHalfHeight - up * farHalfHeight;
        corners[5] = farCenter + right * farHalfHeight - up * farHalfHeight;
        corners[6] = farCenter + right * farHalfHeight + up * farHalfHeight;
        corners[7] = farCenter - right * farHalfHeight + up * farHalfHeight;
    }

    constexpr uint32 edges[12][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
    };

    Render::LineObjectState state = {};
    for (const auto& edge : edges)
    {
        state.lines.push_back({ .start = corners[edge[0]],
                                .end = corners[edge[1]],
                                .color = Color::Green });
    }

    return state;
}

void CameraVisualizer::OnUnitWorldTransformUpdated(EditorVisualContext&, Scene&, Unit&, Component& component)
{
    if (visual.line)
        visual.line->Update(MakeLineState(component));
}

void CameraVisualizer::OnPropertyChanged(EditorVisualContext&, Scene&, Unit&, Component& component, const Property&)
{
    if (visual.line)
        visual.line->Update(MakeLineState(component));
}

void CameraVisualizer::OnSelectionChanged(Unit* previousUnit, Unit* selectedUnit)
{
    if (previousUnit == selectedUnit)
        return;

    Scene* scene = visual.unit->GetOwner();
    Render::SceneSystem& sceneSystem = gEngine->GetRenderSystem().GetSceneSystem();
    Render::RenderScene* currentRenderScene = sceneSystem.GetRenderScene(scene);
    if (!currentRenderScene)
        return;

    EditorVisualContext context = {
        .engine = engine,
        .renderScene = *currentRenderScene
    };

    if (visual.unit == previousUnit)
        DestroyLine(context, visual);
    if (visual.unit == selectedUnit)
        CreateLine(context, *visual.component, visual);
}

void CameraVisualizer::CreateLine(EditorVisualContext& context, Component& component, CameraVisual& visual)
{
    if (visual.line)
        return;

    std::unique_ptr<Render::LineObject> line = std::make_unique<Render::LineObject>(MakeLineState(component));
    visual.line = line.get();

    Render::SceneSystem& sceneSystem = context.renderScene.GetSceneSystem();
    sceneSystem.AddObject(context.renderScene, std::move(line), &component);
}

void CameraVisualizer::DestroyLine(EditorVisualContext& context, CameraVisual& visual)
{
    if (!visual.line)
        return;

    Render::SceneSystem& sceneSystem = context.renderScene.GetSceneSystem();
    sceneSystem.DestroyObject(context.renderScene, visual.line);
    visual.line = nullptr;
}

} // namespace URay
