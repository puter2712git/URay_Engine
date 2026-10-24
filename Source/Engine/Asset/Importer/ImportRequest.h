#pragma once

#include <Core/File/VirtualPath.h>

namespace URay
{

struct ImportRequest
{
    VirtualPath sourcePath;
    VirtualPath outputDirectory;
};

} // namespace URay
