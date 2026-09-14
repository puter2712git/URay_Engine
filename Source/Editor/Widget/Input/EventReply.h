#pragma once

#include "Platform/Window/Cursor.h"

#include <optional>

namespace URay
{

struct EventReply
{
    bool handled = false;

    bool requestFocus = false;

    bool capturePointer = false;
    bool releasePointer = false;

    std::optional<CursorType> cursor;
};

} // namespace URay
