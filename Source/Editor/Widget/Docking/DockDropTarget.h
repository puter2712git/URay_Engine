#pragma once

#include <Engine/Object/ObjectId.h>

namespace URay
{

enum class DockDropZone
{
    Center,
    Left,
    Right,
    Top,
    Bottom
};

struct DockDropTarget
{
    ObjectId groupId;
    DockDropZone zone = DockDropZone::Center;
};

} // namespace URay
