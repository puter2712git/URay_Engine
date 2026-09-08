#pragma once

#include "Render/View/ViewMode.h"

namespace URay::Render
{

struct RenderView
{
    Matrix viewMatrix = Matrix::Identity;
    Matrix projMatrix = Matrix::Identity;

    ViewMode viewMode = ViewMode::Lit;
};

} // namespace URay::Render
