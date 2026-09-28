#pragma once

#include "Engine/Object/Object.h"

#include "Core/UUID.h"

namespace URay
{

using AssetHandle = UUID;
using AssetHandleHash = UUIDHash;

class Asset : public Object
{
    URAY_CLASS(Asset, Object)

public:
    virtual ~Asset() override = default;

public:
    const std::string& GetName() const { return name; }
    void SetName(const std::string& name) { this->name = name; }

    AssetHandle GetHandle() const { return handle; }
    void SetHandle(const AssetHandle& handle) { this->handle = handle; }

private:
    std::string name;
    AssetHandle handle = {};
};

} // namespace URay
