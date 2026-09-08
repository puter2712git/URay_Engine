#pragma once

namespace URay::Render
{

enum class CullMode
{
    Front,
    Back,
    None
};

enum class PolygonMode
{
    Fill,
    Line,
    Point
};

struct RasterizerState
{
    CullMode cullMode = CullMode::Back;
    PolygonMode polygonMode = PolygonMode::Fill;

    bool operator==(const RasterizerState&) const = default;
};

} // namespace URay::Render
