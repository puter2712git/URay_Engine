#pragma once

#include <cstdint>

namespace URay
{

struct AssetHeader
{
    char magic[4];
    uint32_t version = 0;

    uint64_t id = 0;

    uint64_t metadataOffset = 0;
    uint64_t metadataSize = 0;

    uint64_t dataOffset = 0;
    uint64_t dataSize = 0;
};

} // namespace URay
