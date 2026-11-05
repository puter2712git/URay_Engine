#include "Script.h"

namespace URay
{

URAY_REGISTER_CLASS(Script)

void Script::RegisterClass()
{
}

Script::Script(const VirtualPath& filePath, const String& typeName)
    : filePath(filePath), typeName(typeName)
{
}

const VirtualPath& Script::GetFilePath() const
{
    return filePath;
}

const String& Script::GetTypeName() const
{
    return typeName;
}

} // namespace URay
