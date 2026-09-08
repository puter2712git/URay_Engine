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

    const VirtualPath& GetVertexShaderPath() const { return vsPath; }
    void SetVertexShaderPath(const VirtualPath& path) { vsPath = path; }

    const VirtualPath& GetFragmentShaderPath() const { return fsPath; }
    void SetFragmentShaderPath(const VirtualPath& path) { fsPath = path; }

private:
    VirtualPath filePath;

    VirtualPath vsPath;
    VirtualPath fsPath;
};

} // namespace URay
