#pragma once

#include "Core/Math/AABB.h"

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
        uint32_t depth = 0;

        bool IsLeaf() const { return children[0] == -1; }
    };

public:
    Octree(const AABB& rootBounds, uint32_t maxDepth = 7, uint32_t maxObjectCount = 8);
    ~Octree();

public:
    void Insert(Render::BoundedObject* entry);

private:
    bool Insert(int nodeIndex, Render::BoundedObject* entry);

    void Subdivide(int nodeIndex);

private:
    std::vector<Node> nodes;

    uint32_t maxDepth = 7;
    uint32_t maxObjectCount = 8;
};

} // namespace URay
