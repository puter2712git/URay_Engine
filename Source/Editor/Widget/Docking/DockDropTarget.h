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

enum class DockDropKind
{
    Dock,
    Float,
    NoOp
};

struct DockDropResult
{
    DockDropKind kind = DockDropKind::NoOp;
    DockDropTarget target = {};
};

} // namespace URay
