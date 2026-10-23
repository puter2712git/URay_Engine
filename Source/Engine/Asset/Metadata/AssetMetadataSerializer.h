#pragma once

namespace URay
{

class VirtualFileSystem;
class VirtualPath;
struct AssetMetadata;

class AssetMetadataSerializer
{
public:
    AssetMetadataSerializer(VirtualFileSystem& fileSystem);

public:
    bool Save(const AssetMetadata& metadata, const VirtualPath& metadataPath) const;
    bool Load(AssetMetadata& outMetadata, const VirtualPath& metadataPath) const;

private:
    VirtualFileSystem& fileSystem;
};

} // namespace URay
