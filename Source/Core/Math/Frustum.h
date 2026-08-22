#pragma once

#include "Core/Math/AABB.h"
#include "Core/Math/Plane.h"

#include <array>

namespace URay
{

enum class FrustumIntersection
{
    Outside,
    Intersect,
    Inside,
};

enum class FrustumPlane : uint8_t
{
    Left,
    Right,
    Bottom,
    Top,
    Near,
    Far,
};

struct Frustum
{
    std::array<Plane, 6> planes;

    static Frustum FromViewProjection(const Matrix& viewProjection)
    {
        const auto MakePlane = [](float x, float y, float z, float d)
        {
            Plane plane{
                .normal = Vector3(x, y, z),
                .distance = d,
            };

            plane.Normalize();
            return plane;
        };

        Frustum frustum;

        // x + w >= 0, w - x >= 0
        frustum.planes[static_cast<size_t>(FrustumPlane::Left)] =
            MakePlane(
                viewProjection.m00 + viewProjection.m03,
                viewProjection.m10 + viewProjection.m13,
                viewProjection.m20 + viewProjection.m23,
                viewProjection.m30 + viewProjection.m33);

        frustum.planes[static_cast<size_t>(FrustumPlane::Right)] =
            MakePlane(
                viewProjection.m03 - viewProjection.m00,
                viewProjection.m13 - viewProjection.m10,
                viewProjection.m23 - viewProjection.m20,
                viewProjection.m33 - viewProjection.m30);

        // y + w >= 0, w - y >= 0
        frustum.planes[static_cast<size_t>(FrustumPlane::Bottom)] =
            MakePlane(
                viewProjection.m01 + viewProjection.m03,
                viewProjection.m11 + viewProjection.m13,
                viewProjection.m21 + viewProjection.m23,
                viewProjection.m31 + viewProjection.m33);

        frustum.planes[static_cast<size_t>(FrustumPlane::Top)] =
            MakePlane(
                viewProjection.m03 - viewProjection.m01,
                viewProjection.m13 - viewProjection.m11,
                viewProjection.m23 - viewProjection.m21,
                viewProjection.m33 - viewProjection.m31);

        // Vulkan clip space: 0 <= z <= w
        frustum.planes[static_cast<size_t>(FrustumPlane::Near)] =
            MakePlane(
                viewProjection.m02,
                viewProjection.m12,
                viewProjection.m22,
                viewProjection.m32);

        frustum.planes[static_cast<size_t>(FrustumPlane::Far)] =
            MakePlane(
                viewProjection.m03 - viewProjection.m02,
                viewProjection.m13 - viewProjection.m12,
                viewProjection.m23 - viewProjection.m22,
                viewProjection.m33 - viewProjection.m32);

        return frustum;
    }

    FrustumIntersection Intersects(const AABB& aabb) const
    {
        bool fullyInside = true;

        for (const Plane& plane : planes)
        {
            const Vector3 positive = {
                plane.normal.x >= 0.0f ? aabb.max.x : aabb.min.x,
                plane.normal.y >= 0.0f ? aabb.max.y : aabb.min.y,
                plane.normal.z >= 0.0f ? aabb.max.z : aabb.min.z
            };

            if (plane.SignedDistance(positive) < 0.0f)
            {
                return FrustumIntersection::Outside;
            }

            const Vector3 negative = {
                plane.normal.x >= 0.0f ? aabb.min.x : aabb.max.x,
                plane.normal.y >= 0.0f ? aabb.min.y : aabb.max.y,
                plane.normal.z >= 0.0f ? aabb.min.z : aabb.max.z
            };

            if (plane.SignedDistance(negative) < 0.0f)
            {
                fullyInside = false;
            }
        }

        return fullyInside
                   ? FrustumIntersection::Inside
                   : FrustumIntersection::Intersect;
    }
};

} // namespace URay
