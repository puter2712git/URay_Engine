#include "PickSystem.h"

#include "Editor/Editor.h"
#include "Editor/Pick/MeshPickObject.h"
#include "Editor/Pick/PickObject.h"
#include "Editor/Widget/Viewport/GizmoController.h"

#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Component/Render/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Render/RenderSystem.h"
#include "Render/Rendering/Object/Drawable/MeshObject.h"
#include "Render/Rendering/Scene/SceneSystem.h"

#include "Core/Math/Math.h"
#include "Core/Math/Ray.h"
#include "Core/Math/Vector3.h"

namespace URay
{

PickSystem::PickSystem(GizmoController& gizmo) : gizmo(gizmo) {}

PickSystem::~PickSystem() = default;

bool PickSystem::Initialize()
{
    pickRegistry.Register<Render::MeshObject>(
        [](Render::RenderObject& object, Unit& unit)
        {
            return std::make_unique<MeshPickObject>(
                static_cast<Render::MeshObject&>(object), unit);
        });

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::SceneSystem& sceneSystem = renderSystem.GetSceneSystem();

    sceneSystem.GetObjectAddRay().Register(this, [this](Render::RenderScene* scene, Render::RenderObject* object, Unit* unit, Component* component)
                                           { OnRenderObjectAdded(scene, object, unit, component); });
    sceneSystem.GetObjectDestroyRay().Register(this, [this](Render::RenderScene* scene, Render::RenderObject* object)
                                               { OnRenderObjectDestroyed(scene, object); });
    sceneSystem.GetObjectUpdateRay().Register(this, [this](Render::RenderScene* scene, Render::RenderObject* object)
                                              { OnRenderObjectUpdated(scene, object); });

    return true;
}

void PickSystem::Finalize()
{
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::SceneSystem& sceneSystem = renderSystem.GetSceneSystem();

    sceneSystem.GetObjectUpdateRay().UnregisterAll(this);
    sceneSystem.GetObjectDestroyRay().UnregisterAll(this);
    sceneSystem.GetObjectAddRay().UnregisterAll(this);

    pickObjects.clear();
}

void PickSystem::OnRenderObjectAdded(Render::RenderScene* scene, Render::RenderObject* object, Unit* unit, Component* component)
{
    const PickRegistry::Constructor* constructor = pickRegistry.Find(object);
    if (constructor)
    {
        pickObjects.insert_or_assign(object, (*constructor)(*object, *unit));
    }
}

void PickSystem::OnRenderObjectDestroyed(Render::RenderScene* scene, Render::RenderObject* object)
{
    pickObjects.erase(object);
}

void PickSystem::OnRenderObjectUpdated(Render::RenderScene* scene, Render::RenderObject* object)
{
    const auto it = pickObjects.find(object);
    if (it == pickObjects.end())
        return;

    it->second->Update(object);
}

PickResult PickSystem::Pick(
    CameraComponent* camera,
    float screenX,
    float screenY) const
{
    const Vector3 screenNear = Vector3(screenX, screenY, 0.0f);
    const Vector3 screenFar = Vector3(screenX, screenY, 1.0f);

    const Vector3 origin = camera->ScreenToWorld(screenNear);
    const Vector3 farPos = camera->ScreenToWorld(screenFar);
    const Vector3 direction = (farPos - origin).GetNormalized();

    const Ray ray{
        .origin = origin,
        .direction = direction
    };

    PickResult pickResult = {};

    int32 gizmoAxis;
    bool isGizmoPicked = PickGizmo(ray, gizmoAxis);

    if (isGizmoPicked)
    {
        pickResult.isHit = true;
        pickResult.gizmoAxis = gizmoAxis;
        return pickResult;
    }

    float nearestDistance = std::numeric_limits<float>::max();

    for (const auto& [key, obj] : pickObjects)
    {
        bool isHit = obj->Intersect(ray, nearestDistance);

        if (isHit)
        {
            pickResult.isHit = true;
            pickResult.pickedUnit = obj->GetOwner();
        }
    }

    return pickResult;
}

bool PickSystem::PickGizmo(const Ray& worldRay, int32& outAxis) const
{
    if (!gizmo.GetTarget())
        return false;

    const TransformComponent* targetTransform = gizmo.GetTargetTransform();
    if (!targetTransform)
        return false;

    float nearestDistance = std::numeric_limits<float>::max();
    bool finalHit = false;

    for (size_t axis = 0; axis < 3; ++axis)
    {
        const Mesh* mesh = gizmo.GetMesh();

        const std::vector<Render::VertexPNT>& vertices = mesh->GetVertices();
        const std::vector<uint32>& indices = mesh->GetIndices();

        const Matrix gizmoWorld = gizmo.GetWorldMatrix(axis);
        const Matrix invGizmoWorld = gizmoWorld.Inverse();

        const Vector3 localOrigin = invGizmoWorld.TransformPoint(worldRay.origin);
        const Vector3 localDirection = invGizmoWorld.TransformVector(worldRay.direction);

        const Ray localRay{
            .origin = localOrigin,
            .direction = localDirection
        };

        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            const Vector3 p0 = vertices[indices[i]].position;
            const Vector3 p1 = vertices[indices[i + 1]].position;
            const Vector3 p2 = vertices[indices[i + 2]].position;

            float distance;
            bool isHit = Math::IntersectLineTriangle(
                localRay, p0, p1, p2, distance);

            if (!isHit)
                continue;

            if (distance < nearestDistance)
            {
                nearestDistance = distance;
                finalHit = true;
                outAxis = axis;
            }
        }
    }

    return finalHit;
}

} // namespace URay
