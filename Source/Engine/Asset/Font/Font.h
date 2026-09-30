#pragma once

#include "Engine/Asset/Asset.h"

#include "Core/Math/Vector2.h"

#include <string>

namespace URay
{

class Texture;

class Font : public Asset
{
    URAY_CLASS(Font, Asset)

public:
    Font(const std::vector<uint8>& data);
    ~Font() override;

public:
    const std::vector<uint8>& GetData() const { return data; }

private:
    std::vector<uint8> data;
};

} // namespace URay
