#pragma once

#include "Engine/Asset/Asset.h"

#include "Core/File/VirtualPath.h"

namespace URay
{

class VirtualPath;

class Shader : public Asset
{
    URAY_CLASS(Shader, Asset)

public:
    Shader(const VirtualPath& filePath);
    ~Shader();

public:
    const VirtualPath& GetFilePath() const { return filePath; }

private:
    VirtualPath filePath;
};

} // namespace URay
