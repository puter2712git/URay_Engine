#pragma once

namespace URay::Render
{

enum class CullMode
{
    Front,
    Back,
    None
};

struct RasterizerState
{
    CullMode cullMode = CullMode::Back;

    bool operator==(const RasterizerState&) const = default;
};

} // namespace URay::Render
