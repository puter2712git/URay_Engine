#pragma once

#include "Core/Math/Color.h"
#include "Core/Math/Matrix.h"

#include <cstdint>
#include <string>

namespace URay { class Font; class Material; class Mesh; }

namespace URay::RHI
{

struct MeshCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;

    ::URay::Mesh* meshAsset = nullptr;

    ::URay::Material* material = nullptr;
};

struct LineCommandContext
{
    Vector3 start = Vector3::Zero;
    Vector3 end = Vector3::Zero;
    Color color = Color::White;
};

struct TextCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    std::string text;
    ::URay::Font* font = nullptr;
};

struct GizmoCommandContext
{
    Matrix worldMatrix = Matrix::Identity;
    Color colorTint = Color::White;

    ::URay::Mesh* meshAsset = nullptr;

    ::URay::Material* material = nullptr;
};

} // namespace URay::RHI
