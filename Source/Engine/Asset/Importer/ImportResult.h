#pragma once

#include "Engine/Asset/AssetMetadata.h"

namespace URay
{

class Object;

struct ImportResult
{
    Object* assetObject;
    AssetMetadata metadata = {};
};

} // namespace URay
