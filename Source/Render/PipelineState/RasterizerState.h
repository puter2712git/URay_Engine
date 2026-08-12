#pragma once

namespace URay::RHI
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

} // namespace URay::RHI
