#pragma once

#include "Engine/Asset/Texture/TextureCookData.h"

#include "Core/Type/Types.h"

#include <vector>

namespace URay
{

class TextureSerializer
{
public:
    std::vector<uint8> Serialize(const TextureCookData& cookData) const;
    TextureCookData Deserialize(const std::vector<uint8>& bytes) const;

private:
    void WriteBytes(std::vector<uint8>& bytes, uint32 value) const;
    uint32 ReadBytes(const std::vector<uint8>& bytes, size_t offset) const;
};

} // namespace URay
