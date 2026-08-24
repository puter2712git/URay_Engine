#include "EditorPicker.h"

#include "Editor/GizmoController.h"

#include "Core/Math/Math.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Ray.h"
#include "Core/Math/Vector3.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/MeshComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Unit.h"

namespace URay
{

EditorPicker::EditorPicker(Engine& engine, GizmoController* gizmo)
    : engine(engine), gizmo(gizmo)
{
}

PickResult EditorPicker::Pick(CameraComponent* camera, float screenX, float screenY) const
{
    const Vector3 screenNear = Vector3(screenX, screenY, 0.0f);
    const Vector3 screenFar = Vector3(screenX, screenY, 1.0f);

    const Vector3 origin = camera->ScreenToWorld(screenNear);

    const Vector3 farPos = camera->ScreenToWorld(screenFar);
    const Vector3 direction = (farPos - origin).GetNormalized();

    Ray ray = Ray(origin, direction);

    PickResult result = {};

    int gizmoAxis;
    bool isGizmoPicked = PickGizmo(ray, gizmoAxis);

    if (isGizmoPicked)
    {
        result.hit = true;
        result.gizmoAxis = gizmoAxis;
        return result;
    }

    float minDist = std::numeric_limits<float>::max();

    const Scene* scene = engine.GetSceneByType(SceneType::Game);
    const std::vector<Unit*> units = scene->GetUnits();

    for (Unit* unit : units)
    {
        std::vector<Component*> components = unit->GetComponents();

        for (const Component* comp : components)
        {
            bool isHit = false;

            if (const MeshComponent* meshComp = dynamic_cast<const MeshComponent*>(comp))
            {
                isHit = PickMesh(ray, meshComp, minDist);
            }

            if (isHit)
            {
                result.hit = true;
                result.pickedUnit = unit;
            }
        }
    }

    return result;
}

bool EditorPicker::PickGizmo(const Ray& ray, int& outAxis) const
{
    if (!gizmo || !gizmo->GetTarget())
        return false;

    const TransformComponent* targetTransform = gizmo->GetTargetTransform();
    if (!targetTransform)
        return false;

    float minDist = std::numeric_limits<float>::max();
    bool isHit = false;

    for (size_t axis = 0; axis < 3; ++axis)
    {
        const Mesh* meshAsset = gizmo->GetMesh();

        const std::vector<RHI::VertexPNT>& vertices = meshAsset->GetVertices();
        const std::vector<uint32_t>& indices = meshAsset->GetIndices();

        const Matrix gizmoWorld = gizmo->GetWorldMatrix(axis);
        const Matrix invGizmoWorld = gizmoWorld.Inverse();

        const Vector3 localOrigin = invGizmoWorld.TransformPoint(ray.origin);
        const Vector3 localDirection = invGizmoWorld.TransformVector(ray.direction);

        const Ray localRay = Ray(localOrigin, localDirection);

        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            const Vector3 p0 = vertices[indices[i]].position;
            const Vector3 p1 = vertices[indices[i + 1]].position;
            const Vector3 p2 = vertices[indices[i + 2]].position;

            float dist;
            bool hit = Math::IntersectLineTriangle(localRay, p0, p1, p2, dist);

            if (!hit)
                continue;

            if (dist < minDist)
            {
                minDist = dist;
                isHit = true;
                outAxis = axis;
            }
        }
    }

    return isHit;
}

bool EditorPicker::PickMesh(const Ray& ray, const MeshComponent* meshComp, float& minDist) const
{
    Unit* owner = meshComp->GetOwner();

    TransformComponent* transform = owner->GetTransform();
    if (!transform)
        return false;

    const Mesh* mesh = meshComp->GetMesh();
    if (!mesh)
        return false;

    const std::vector<RHI::VertexPNT>& vertices = mesh->GetVertices();
    const std::vector<uint32_t>& indices = mesh->GetIndices();

    bool isHit = false;

    const Vector3 localOrigin = transform->InvTransformPoint(ray.origin);
    const Vector3 localDirection = transform->InvTransformVector(ray.direction);

    const Ray& localRay = Ray(localOrigin, localDirection);

    float boxDistance = 0.0f;
    if (!Math::IntersectRayAABB(localRay, mesh->GetLocalBounds(), boxDistance))
        return false;

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        const Vector3 p0 = vertices[indices[i]].position;
        const Vector3 p1 = vertices[indices[i + 1]].position;
        const Vector3 p2 = vertices[indices[i + 2]].position;

        float dist;
        bool hit = Math::IntersectLineTriangle(
            localRay,
            p0, p1, p2,
            dist);

        if (!hit)
            continue;

        if (dist < minDist)
        {
            minDist = dist;
            isHit = true;
        }
    }

    return isHit;
}

} // namespace URay
