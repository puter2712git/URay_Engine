#pragma once

#include "Core/Math/AABB.h"
#include "Core/Math/Frustum.h"
#include "Core/Type/Types.h"

#include <array>
#include <vector>

namespace URay
{

namespace Render
{
class BoundedObject;
class DrawCommandBuilder;
} // namespace Render

class Octree
{
private:
    struct Node
    {
        AABB bounds = {};
        std::array<int, 8> children = { -1, -1, -1, -1, -1, -1, -1, -1 };
        std::vector<Render::BoundedObject*> entries;
        uint32 depth = 0;

        bool IsLeaf() const { return children[0] == -1; }
    };

public:
    Octree(const AABB& rootBounds, uint32 maxDepth = 7, uint32 maxObjectCount = 8);
    ~Octree();

public:
    void Insert(Render::BoundedObject* entry);

    void Query(const Frustum& frustum,
               std::vector<Render::BoundedObject*>& results) const;

private:
    bool Insert(int nodeIndex, Render::BoundedObject* entry);

    void Subdivide(int nodeIndex);

    void Query(int nodeIndex,
               const Frustum& frustum,
               std::vector<Render::BoundedObject*>& results) const;

    void Collect(int nodeIndex,
                 std::vector<Render::BoundedObject*>& results) const;

private:
    std::vector<Node> nodes;

    uint32 maxDepth = 7;
    uint32 maxObjectCount = 8;
};

} // namespace URay
