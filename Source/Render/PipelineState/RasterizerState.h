#pragma once

namespace URay
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
};

} // namespace URay
