#pragma once

#include <Core/Type/TypeAlias.h>
#include <Core/File/VirtualPath.h>

namespace URay
{
class AssetSystem;

class ScriptBuilder
{
public:
    explicit ScriptBuilder(AssetSystem& assetSystem) : assetSystem(assetSystem) {}
    bool Build(const VirtualPath& projectPath, Vector<String>& typeNames) const;

private:
    bool CollectInputs(Vector<VirtualPath>& paths, Vector<String>& typeNames) const;
    AssetSystem& assetSystem;
};
} // namespace URay
