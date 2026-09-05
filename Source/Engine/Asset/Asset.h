#pragma once

#include "Engine/Object/Object.h"

#include "Core/UUID.h"

namespace URay
{

class Asset : public Object
{
    URAY_CLASS(Asset, Object)

public:
    const std::string& GetName() const { return name; }
    void SetName(const std::string& name) { this->name = name; }

    UUID GetUUID() const { return uuid; }
    void SetUUID(const UUID& uuid) { this->uuid = uuid; }

private:
    std::string name;
    UUID uuid = {};
};

} // namespace URay
