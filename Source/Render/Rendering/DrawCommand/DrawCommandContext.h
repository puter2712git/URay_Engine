#pragma once

#include <Core/Math/AABB.h>
#include <Core/Math/Color.h>
#include <Core/Math/Matrix.h>
#include <Core/Type/TypeAlias.h>
#include <Render/Rendering/View/ViewMode.h>

#include <cstdint>

namespace URay
{
class MaterialInterface;
class Mesh;
class SkinnedMesh;
} // namespace URay

namespace URay::Render
{

class DescriptorSet;

struct DrawCommandParams
{
    ViewMode viewMode = ViewMode::Lit;
};

struct MeshCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;
    uint32 objectId = 0;

    Mesh* mesh = nullptr;
    URay::MaterialInterface* material = nullptr;

    uint32 indexOffset = 0;
    uint32 indexCount = 0;
};

struct SkinnedMeshCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;
    uint32 objectId = 0;

    SkinnedMesh* skinnedMesh = nullptr;
    URay::MaterialInterface* material = nullptr;
    DescriptorSet* skinningDescriptorSet = nullptr;

    uint32 indexOffset = 0;
    uint32 indexCount = 0;
};

struct BillboardCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;
    uint32 objectId = 0;

    Mesh* mesh = nullptr;
    URay::MaterialInterface* material = nullptr;

    uint32 indexOffset = 0;
    uint32 indexCount = 0;
};

struct LineCommandContext
{
    Vector3 start = Vector3::Zero;
    Vector3 end = Vector3::Zero;
    Color color = Color::White;
};

struct DecalCommandContext
{
    Matrix meshWorldMatrix = Matrix::Identity;
    Mesh* receiverMesh = nullptr;
    uint32 indexOffset = 0;
    uint32 indexCount = 0;

    URay::MaterialInterface* decalMaterial = nullptr;
    DescriptorSet* decalDescriptorSet = nullptr;
};

struct GizmoCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;
    Mesh* mesh = nullptr;
    URay::MaterialInterface* material = nullptr;
};

} // namespace URay::Render
