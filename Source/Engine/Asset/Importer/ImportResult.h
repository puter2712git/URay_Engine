#pragma once

#include <Core/File/VirtualPath.h>
#include <Engine/Asset/Metadata/AssetMetadata.h>

#include <string>
#include <vector>

namespace URay
{

struct ImportResult
{
    bool succeeded = false;
    std::string errorMessage;

    std::vector<AssetMetadata> outputMetadatas;
    std::vector<VirtualPath> dependencies;

    explicit operator bool() const { return succeeded; }

    static ImportResult Success()
    {
        ImportResult result;
        result.succeeded = true;
        return result;
    }

    static ImportResult Failure(const std::string& errorMessage)
    {
        ImportResult result;
        result.succeeded = false;
        result.errorMessage = errorMessage;
        return result;
    }
};

} // namespace URay
