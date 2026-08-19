#pragma once

namespace URay
{

struct EventReply
{
    bool handled = false;

    bool requestFocus = false;

    bool capturePointer = false;
    bool releasePointer = false;
};

} // namespace URay
