#include "LogAPI.h"

#include <Core/Log/LogSystem.h>

namespace URay
{

namespace
{

void __cdecl NativeLog(const char* message) noexcept
{
    if (!message)
        return;

    try
    {
        URAY_LOG("[C#] %s", message);
    }
    catch (...)
    {
    }
}

} // namespace

LogAPI CreateLogAPI()
{
    return LogAPI{
        .Log = &NativeLog
    };
}

} // namespace URay
