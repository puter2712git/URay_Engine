#pragma once

#include <Core/Type/TypeAlias.h>
#include <Core/Math/Vector2.h>
#include <Engine/Asset/Asset.h>

#include <string>

namespace URay
{

class Texture;

class Font : public Asset
{
    URAY_OBJECT(Font, Asset)

public:
    Font(const Vector<uint8>& data);
    ~Font() override;

public:
    const Vector<uint8>& GetData() const { return data; }

private:
    Vector<uint8> data;
};

} // namespace URay
