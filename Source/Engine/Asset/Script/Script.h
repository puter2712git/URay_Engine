#pragma once

#include <Core/File/VirtualPath.h>
#include <Engine/Asset/Asset.h>

namespace URay
{

class Script : public Asset
{
    URAY_OBJECT(Script, Asset)

public:
    Script(const VirtualPath& filePath, const String& typeName);

public:
    const VirtualPath& GetFilePath() const;
    const String& GetTypeName() const;

private:
    VirtualPath filePath;
    String typeName;
};

} // namespace URay
