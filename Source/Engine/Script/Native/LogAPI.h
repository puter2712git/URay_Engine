#pragma once

namespace URay
{

struct LogAPI
{
    void(__cdecl* Log)(const char* message);
};

LogAPI CreateLogAPI();

} // namespace URay
