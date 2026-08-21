#include "Octree.h"

#include <stack>

namespace URay
{

Octree::Octree()
{
    root.bounds = {
        .min = Vector3(-100.0f, -100.0f, -100.0f),
        .max = Vector3(100.0f, 100.0f, 100.0f),
    };
}

Octree::~Octree() = default;

void Octree::Clear()
{
    root = {};
}

void Octree::Insert(MeshComponent* entry, const AABB& worldBounds)
{
}

} // namespace URay
