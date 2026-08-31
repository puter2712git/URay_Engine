#pragma once

namespace URay::Render
{

class ImGuiDrawable
{
public:
    virtual ~ImGuiDrawable() = default;

public:
    virtual void DrawImGui() = 0;
};

} // namespace URay::Render
