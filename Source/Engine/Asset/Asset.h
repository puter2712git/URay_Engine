#pragma once

#include <Engine/Object/Object.h>

#include <Engine/Asset/AssetHandle.h>

namespace URay
{

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
