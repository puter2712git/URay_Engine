#include "Octree.h"

#include "Engine/Component/Render/MeshComponent.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"

#include <stack>

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

void Octree::Insert(MeshComponent* entry)
{
    if (!entry || nodes.empty())
        return;

    Insert(0, entry);
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

void Octree::BuildDebugLines(RHI::DrawCommandBuilder& builder) const
{
    for (const Node& node : nodes)
    {
        const Vector3& min = node.bounds.min;
        const Vector3& max = node.bounds.max;

        const Vector3& depthColor = depthColors[node.depth];
        const Color color = Color(depthColor.x, depthColor.y, depthColor.z, 1.0f);

        builder.BuildFromLine({ { min.x, min.y, min.z }, { max.x, min.y, min.z }, color });
        builder.BuildFromLine({ { min.x, max.y, min.z }, { max.x, max.y, min.z }, color });
        builder.BuildFromLine({ { min.x, min.y, max.z }, { max.x, min.y, max.z }, color });
        builder.BuildFromLine({ { min.x, max.y, max.z }, { max.x, max.y, max.z }, color });

        builder.BuildFromLine({ { min.x, min.y, min.z }, { min.x, max.y, min.z }, color });
        builder.BuildFromLine({ { max.x, min.y, min.z }, { max.x, max.y, min.z }, color });
        builder.BuildFromLine({ { min.x, min.y, max.z }, { min.x, max.y, max.z }, color });
        builder.BuildFromLine({ { max.x, min.y, max.z }, { max.x, max.y, max.z }, color });

        builder.BuildFromLine({ { min.x, min.y, min.z }, { min.x, min.y, max.z }, color });
        builder.BuildFromLine({ { max.x, min.y, min.z }, { max.x, min.y, max.z }, color });
        builder.BuildFromLine({ { min.x, max.y, min.z }, { min.x, max.y, max.z }, color });
        builder.BuildFromLine({ { max.x, max.y, min.z }, { max.x, max.y, max.z }, color });
    }
}

bool Octree::Insert(int nodeIndex, MeshComponent* entry)
{
    const AABB entryBounds = {};

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

    std::vector<MeshComponent*> oldEntries = std::move(nodes[nodeIndex].entries);

    for (MeshComponent* entry : oldEntries)
    {
        const AABB& entryBounds = {};

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
