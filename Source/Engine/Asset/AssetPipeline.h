#pragma once

#include <string>
#include <unordered_map>

namespace URay
{

class VirtualFilesystem;
class VirtualPath;

class Importer;

class AssetPipeline
{
public:
    AssetPipeline(VirtualFilesystem& filesystem);
    ~AssetPipeline();

public:
    void Import(const VirtualPath& path);

private:
    VirtualFilesystem& filesystem;

    std::unordered_map<std::string, Importer*> importers;
};

} // namespace URay
