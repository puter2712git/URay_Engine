#pragma once

namespace URay
{

class VirtualFileSystem;
class VirtualPath;
struct MaterialData;

class MaterialSerializer
{
public:
    MaterialSerializer(VirtualFileSystem& fileSystem);

public:
    bool Save(const MaterialData& material, const VirtualPath& assetPath) const;
    bool Load(MaterialData& outMaterial, const VirtualPath& assetPath) const;

private:
    VirtualFileSystem& fileSystem;
};

} // namespace URay
