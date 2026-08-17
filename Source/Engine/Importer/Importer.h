#pragma once

#include <string>
#include <vector>

namespace URay
{

class Object;
class VirtualPath;

struct ImportResult
{
    std::vector<Object*> objects;
    std::vector<std::string> errorMsgs;
};

class Importer
{
public:
    virtual ~Importer() = default;

public:
    virtual ImportResult Import(const VirtualPath& filePath) = 0;
};

} // namespace URay
