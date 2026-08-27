#include "Octree.h"

#include "Render/Scene/Object/BoundedObject.h"

namespace URay
{

Octree::Octree(const AABB& rootBounds, uint32_t maxDepth, uint32_t maxObjectCount)
    : maxDepth(maxDepth), maxObjectCount(maxObjectCount)
{
    nodes.clear();
    nodes.push_back(Node{
        .bounds = rootBounds,
        .depth = 0,
    });
}

Octree::~Octree()
{
}

void Octree::Insert(Render::BoundedObject* entry)
{
    if (!entry || nodes.empty())
        return;

    Insert(0, entry);
}

void Octree::Query(const Frustum& frustum,
                   std::vector<Render::BoundedObject*>& results) const
{
    if (nodes.empty())
        return;

    Query(0, frustum, results);
}

void Octree::Query(int nodeIndex,
                   const Frustum& frustum,
                   std::vector<Render::BoundedObject*>& results) const
{
    const Node& node = nodes[nodeIndex];
    const FrustumIntersection intersection = frustum.Intersects(node.bounds);

    if (intersection == FrustumIntersection::Outside)
        return;

    if (intersection == FrustumIntersection::Inside)
    {
        Collect(nodeIndex, results);
        return;
    }

    for (Render::BoundedObject* entry : node.entries)
    {
        if (frustum.Intersects(entry->GetWorldBounds()) !=
            FrustumIntersection::Outside)
        {
            results.push_back(entry);
        }
    }

    if (!node.IsLeaf())
    {
        for (int childIndex : node.children)
        {
            Query(childIndex, frustum, results);
        }
    }
}

void Octree::Collect(int nodeIndex,
                     std::vector<Render::BoundedObject*>& results) const
{
    const Node& node = nodes[nodeIndex];

    results.insert(results.end(), node.entries.begin(), node.entries.end());

    if (!node.IsLeaf())
    {
        for (int childIndex : node.children)
        {
            Collect(childIndex, results);
        }
    }
}

static const std::array<Vector3, 8> depthColors = {
    Vector3{ 1.0f, 0.0f, 0.0f }, // red
    Vector3{ 0.0f, 1.0f, 0.0f }, // green
    Vector3{ 0.0f, 0.4f, 1.0f }, // blue
    Vector3{ 1.0f, 1.0f, 0.0f }, // yellow
    Vector3{ 1.0f, 0.0f, 1.0f }, // magenta
    Vector3{ 0.0f, 1.0f, 1.0f }, // cyan
    Vector3{ 1.0f, 0.5f, 0.0f }, // orange
    Vector3{ 0.7f, 0.3f, 1.0f }, // purple
};

bool Octree::Insert(int nodeIndex, Render::BoundedObject* entry)
{
    const AABB& entryBounds = entry->GetWorldBounds();

    if (!nodes[nodeIndex].bounds.Contains(entryBounds))
        return false;

    if (!nodes[nodeIndex].IsLeaf())
    {
        for (int childIndex : nodes[nodeIndex].children)
        {
            if (Insert(childIndex, entry))
                return true;
        }

        nodes[nodeIndex].entries.push_back(entry);
        return true;
    }

    nodes[nodeIndex].entries.push_back(entry);

    if (nodes[nodeIndex].entries.size() > maxObjectCount &&
        nodes[nodeIndex].depth < maxDepth)
    {
        Subdivide(nodeIndex);
    }

    return true;
}

void Octree::Subdivide(int nodeIndex)
{
    const AABB parentBounds = nodes[nodeIndex].bounds;
    const uint32_t childDepth = nodes[nodeIndex].depth + 1;

    const Vector3 center = (parentBounds.min + parentBounds.max) * 0.5f;

    std::array<int, 8> childIndices;

    for (int i = 0; i < 8; ++i)
    {
        AABB childBounds = {};

        childBounds.min.x = (i & 1) ? center.x : parentBounds.min.x;
        childBounds.max.x = (i & 1) ? parentBounds.max.x : center.x;

        childBounds.min.y = (i & 2) ? center.y : parentBounds.min.y;
        childBounds.max.y = (i & 2) ? parentBounds.max.y : center.y;

        childBounds.min.z = (i & 4) ? center.z : parentBounds.min.z;
        childBounds.max.z = (i & 4) ? parentBounds.max.z : center.z;

        childIndices[i] = static_cast<int>(nodes.size());

        nodes.push_back(Node{
            .bounds = childBounds,
            .depth = childDepth,
        });
    }

    nodes[nodeIndex].children = childIndices;

    std::vector<Render::BoundedObject*> oldEntries = std::move(nodes[nodeIndex].entries);

    for (Render::BoundedObject* entry : oldEntries)
    {
        const AABB& entryBounds = entry->GetWorldBounds();

        bool moved = false;

        for (int childIndex : childIndices)
        {
            if (nodes[childIndex].bounds.Contains(entryBounds))
            {
                nodes[childIndex].entries.push_back(entry);
                moved = true;
                break;
            }
        }

        if (!moved)
        {
            nodes[nodeIndex].entries.push_back(entry);
        }
    }
}

} // namespace URay
