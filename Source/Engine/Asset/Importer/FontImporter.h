#pragma once

#include "Engine/Asset/Importer/Importer.h"

namespace URay
{

class FontImporter : public Importer
{
public:
    FontImporter();
    ~FontImporter() override;

public:
    void Import(const VirtualPath& sourcePath) override;

    AssetMetadata CreateMetadata(const VirtualPath& sourcePath) const;

    bool CanImport(const std::string& extension) const override;
};

} // namespace URay
