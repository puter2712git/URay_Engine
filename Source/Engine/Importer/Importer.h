#pragma once

#include "Core/Error/Value.h"

#include <vector>

namespace URay
{

class Object;
class VirtualPath;

struct ImportResult
{
    std::vector<Value<Object*>> objects;
};

class Importer
{
public:
    virtual ~Importer() = default;

public:
    virtual ImportResult Import(const VirtualPath& filePath) = 0;
};

} // namespace URay
