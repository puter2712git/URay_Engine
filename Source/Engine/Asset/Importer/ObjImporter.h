#pragma once

#include "Engine/Asset/Importer/Importer.h"

#include "Core/File/VirtualPath.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"
#include "Core/Type/Types.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace URay
{

class Material;

class VirtualFileSystem;

class OBJImporter : public Importer
{
public:
    OBJImporter();
    ~OBJImporter();

public:
    void Import(const VirtualPath& sourcePath) override;

    AssetMetadata CreateMetadata(const VirtualPath& sourcePath) const override;

    bool CanImport(const std::string& extension) const override;
};

} // namespace URay
