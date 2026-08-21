#pragma once

#include "Core/Math/AABB.h"

#include <array>
#include <vector>

namespace URay
{

class MeshComponent;

class Octree
{
private:
    struct Node
    {
        AABB bounds = {};
        std::array<int, 8> children;
        std::vector<MeshComponent*> entries;
        uint32_t depth = 0;
    };

public:
    Octree();
    ~Octree();

public:
    void Clear();

    void Insert(MeshComponent* entry, const AABB& worldBounds);

private:
    Node root = {};
};

} // namespace URay
