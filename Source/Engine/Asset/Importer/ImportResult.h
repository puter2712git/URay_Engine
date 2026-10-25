#pragma once
#include <Core/Type/Types.h>

#include <Core/File/VirtualPath.h>
#include <Engine/Asset/Metadata/AssetMetadata.h>

#include <vector>

namespace URay
{

struct DependencyResult
{
    bool succeeded = false;
    String errorMessage;

    std::vector<VirtualPath> sourcePaths;

    explicit operator bool() const { return succeeded; }

    static DependencyResult Success()
    {
        DependencyResult result;
        result.succeeded = true;
        return result;
    }

    static DependencyResult Failure(const String& errorMessage)
    {
        DependencyResult result;
        result.succeeded = false;
        result.errorMessage = errorMessage;
        return result;
    }
};

struct ImportResult
{
    bool succeeded = false;
    String errorMessage;

    std::vector<AssetMetadata> metadatas;

    explicit operator bool() const { return succeeded; }

    static ImportResult Success()
    {
        ImportResult result;
        result.succeeded = true;
        return result;
    }

    static ImportResult Failure(const String& errorMessage)
    {
        ImportResult result;
        result.succeeded = false;
        result.errorMessage = errorMessage;
        return result;
    }
};

} // namespace URay
