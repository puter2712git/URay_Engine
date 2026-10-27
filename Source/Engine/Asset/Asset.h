#pragma once
#include <Core/Type/TypeAlias.h>

#include <Engine/Object/Object.h>

#include <Engine/Asset/AssetHandle.h>

namespace URay
{

class Asset : public Object
{
    URAY_OBJECT(Asset, Object)

public:
    virtual ~Asset() override = default;

public:
    const String& GetName() const { return name; }
    void SetName(const String& name) { this->name = name; }

    AssetHandle GetHandle() const { return handle; }
    void SetHandle(const AssetHandle& handle) { this->handle = handle; }

private:
    String name;
    AssetHandle handle = {};
};

} // namespace URay
