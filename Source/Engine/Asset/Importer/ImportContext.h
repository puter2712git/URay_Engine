#pragma once

#include "Core/UUID.h"

namespace URay
{

class AssetSystem;

class ImportContext
{
public:
    ImportContext(AssetSystem& assetSystem);
    ~ImportContext();

public:
    AssetSystem& GetAssetSystem() const { return assetSystem; }

private:
    AssetSystem& assetSystem;
};

} // namespace URay
