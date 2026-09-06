#pragma once

#include "Engine/Asset/Material/MaterialCookData.h"

#include "Core/Type/Types.h"

#include <string>
#include <vector>

namespace URay
{

class MaterialSerializer
{
public:
    std::vector<uint8> Serialize(const MaterialCookData& data) const;
    bool Deserialize(const std::vector<uint8>& bytes, MaterialCookData& data) const;

private:
    void WriteUInt32(std::vector<uint8>& bytes, uint32 value) const;
    uint32 ReadUInt32(const std::vector<uint8>& bytes, size_t offset) const;
    void WriteFloat(std::vector<uint8>& bytes, float value) const;
    float ReadFloat(const std::vector<uint8>& bytes, size_t offset) const;
    void WriteString(std::vector<uint8>& bytes, const std::string& value) const;
};

} // namespace URay
