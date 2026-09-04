#pragma once

#include "Engine/Object/Object.h"

#include "Core/UUID.h"

namespace URay
{

class Asset : public Object
{
    URAY_CLASS(Asset, Object)

public:
    UUID GetUUID() const { return uuid; }
    void SetUUID(const UUID& newUUID) { uuid = newUUID; }

private:
    UUID uuid = {};
};

} // namespace URay
